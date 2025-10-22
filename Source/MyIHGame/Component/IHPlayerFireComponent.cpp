// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/IHPlayerFireComponent.h"
#include "Character/IHPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/IHPlayerAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "IHPlayerMoveComponent.h"
#include "Camera/CameraComponent.h"
#include "Util/IHDefine.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/IHEnemyFSM.h"
#include "Weapon/IHBullet.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/IHWeapon.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Util/MyIHGame.h"
#include "Data/IHDataSubsystem.h"
#include "Data/IHInputDataAsset.h"
#include "IHGameplayTags.h"
#include "../Weapon/TPS_BaseWeaponProjectile.h"
#include <Net/UnrealNetwork.h>
#include "../UI/IHHUD.h"

namespace IHConsoleVariables
{
	static bool DrawCrosshairLine = false;
	static FAutoConsoleVariableRef CVarDrawCrosshairLine(
		TEXT("IH.DrawCrosshairLine"),
		DrawCrosshairLine,
		TEXT("debug drawing for crosshair line"),
		ECVF_Default);
}

UIHPlayerFireComponent::UIHPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 컴포넌트도 복제를 시켜줘야. 비신뢰성 RPC 호출이 잘된다.
	SetIsReplicatedByDefault(true);
}

void UIHPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	CamComp = OwnerCharacter->CamComp;

	if (GetOwner()->HasAuthority())
	{
		if (StartWeapon)	// 무기를 블루프린트에서 지정했다면
		{
			//월드에 스폰시켜서 Weapon 이라는 변수에 저장한다.
			Weapon = GetWorld()->SpawnActor<AIHWeapon>(StartWeapon);
			Weapon->SetOwner(OwnerCharacter);

			// 무기가 캐릭터 오른손을 따라다니게 설정한다.
			// 여러분은 hand_rSocket
			const USkeletalMeshSocket* HandSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
			if (HandSocket)
			{
				HandSocket->AttachActor(Weapon, OwnerCharacter->GetMesh());
			}
		}
	}

	ProjectileClass = ATPS_BaseWeaponProjectile::StaticClass();
}

void UIHPlayerFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 카메라가 캐릭터와 너무 가까우면 캐릭터를 숨김처리
	CheckCameraVisible();

	// 에임 오프셋
	UpdateAimOffset(DeltaTime);

	if (OwnerCharacter->IsLocallyControlled())
	{
		CheckCrosshairTarget();

		UpdtaeCrosshairSpread(DeltaTime);
	}
}

void UIHPlayerFireComponent::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	// 싱글톤 GameInstance 가져오기
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UIHDataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UIHDataSubsystem>();
	if (DataSubsystem == nullptr)
		return;

	UIHInputDataAsset* InputData = DataSubsystem->InputDataAsset;
	if (InputData == nullptr)
		return;

	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Fire), ETriggerEvent::Triggered, this, &UIHPlayerFireComponent::InputFire);
	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Fire), ETriggerEvent::Completed, this, &UIHPlayerFireComponent::CompleteInputFire);
	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_DrawKnife), ETriggerEvent::Triggered, this, &UIHPlayerFireComponent::InputThrowingAction);
}

// 에임 오프셋
void UIHPlayerFireComponent::UpdateAimOffset(float DeltaTime)
{
	// 이동중일때는 원래 카메라 따라서 이동
	// 가만히 서있을때만 AimOffset 적용
	FVector Velocity = OwnerCharacter->GetVelocity();
	Velocity.Z = 0;	// 높이값은 계산에서 제외

	// 공중에서 떨어지거나 점프중도 제외
	bool bIsInAir = OwnerCharacter->GetCharacterMovement()->IsFalling();

	// Velocity = Speed * Dir
	float Speed = Velocity.Size();

	// 가만히 서있을때만 AO 계산
	if (Speed == 0.f && bIsInAir == false)
	{
		// 카메라의 회전값을 따라서 AimOffset 값을 계산한다.
		// 컨트롤러의 회전값 = 카메라의 회전값
		// 컨트롤러 회전 yaw 사용 옵션을 체크해서
		float CurrentAimYaw = GetRemoveControlYaw();
		float DeltaYaw = FMath::FindDeltaAngleDegrees(AimOffset_StartYaw, CurrentAimYaw);
		// -180~180
		AimOffset_Yaw = DeltaYaw;

		// 일시적으로 카메라=캐릭터의 회전 싱크를 끊는다.
		//OwnerCharacter->bUseControllerRotationYaw = false;

		TurnInPlace(DeltaTime);

		//PRINT_LOG(TEXT("AO_StartYaw : %f, AO_Yaw : %f"), AimOffset_StartYaw, AimOffset_Yaw);
	}
	// 이동중일떄는 카메라 적용
	if (Speed > 0 || bIsInAir)
	{
		// 이동중일때는 카메라의 회전값 = AimOffset의 기준값
		AimOffset_StartYaw = GetRemoveControlYaw();
		AimOffset_Yaw = 0;	// Yaw값은 가만히 서있을때만 갱신된다.

		OwnerCharacter->bUseControllerRotationYaw = true;

		TurnInPlaceType = ETurnInPlace::ETIP_NotTurning;
	}

	// Pitch
	AimOffset_Pitch = OwnerCharacter->GetBaseAimRotation().Pitch;

	//네트워크 데이터 패킹 과정에서, 음수 각도가 360 - r 만큼으로 환전되어 전달됨
	if (AimOffset_Pitch > 90.0f && OwnerCharacter->IsLocallyControlled() == false)
	{
		FVector2D InRange(270, 360);
		FVector2D OutRange(-90, 0);

		AimOffset_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffset_Pitch);
	}

}

void UIHPlayerFireComponent::InputFire(const FInputActionValue& InputValue)
{
	if (!OwnerCharacter)
	{
		return;
	}

	if (!canFire)
		return;

	{
		FVector StartPos = CamComp->GetComponentLocation();

		FVector RandomDir = GetRandomAimDirection();

		LocalFire(StartPos, RandomDir);

		InputFire_Server(StartPos, RandomDir);

		if (OwnerCharacter->IsLocallyControlled())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Shotter Data]  Input Fire : [StartPos] %f  %f , [RandomDir] %f  %f "), StartPos.X, StartPos.Y, RandomDir.X, RandomDir.Y);
		}
	}

	canFire = false;

	if (Weapon)
	{
		GetWorld()->GetTimerManager().SetTimer(rifleAttackTimerHandler, FTimerDelegate::CreateLambda([this]() {canFire = true; }), 1 / Weapon->AttackSpeed, false);
	}

	
}

void UIHPlayerFireComponent::CompleteInputFire(const FInputActionValue& InputValue)
{
	GetWorld()->GetTimerManager().ClearTimer(rifleAttackTimerHandler);
	canFire = true;
}

void UIHPlayerFireComponent::SetProjectileClass(TSubclassOf<class ATPS_BaseWeaponProjectile> NewProjectileClass)
{
	ProjectileClass = NewProjectileClass;
}

void UIHPlayerFireComponent::InputThrowingAction()
{
	UIHPlayerAnimInstance* AnimInstance = Cast<UIHPlayerAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayDrawingAnim();
	}

	ThrowingAction_Server2();

	/*if (GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		ThrowingAnimPlay_Multicast();
	}

	else
	{
		ThrowingAction_Server2();
	}*/

}

void UIHPlayerFireComponent::TurnInPlace(float deltaTime)
{
	if (TurnInPlaceType == ETurnInPlace::ETIP_NotTurning)
	{
		AimOFfset_InterpYaw = AimOffset_Yaw;
	}


	if (AimOffset_Yaw < -TurnInPlaceAngle)
	{
		TurnInPlaceType = ETurnInPlace::ETIP_Left;
	}

	else if (AimOffset_Yaw > TurnInPlaceAngle)
	{
		TurnInPlaceType = ETurnInPlace::ETIP_Right;
	}

	if (TurnInPlaceType != ETurnInPlace::ETIP_NotTurning)
	{
		AimOFfset_InterpYaw = FMath::FInterpTo(AimOffset_Yaw, 0.0f, deltaTime, TurnInPlaceTime);
		AimOffset_Yaw = AimOFfset_InterpYaw;

		if (FMath::Abs<float>(AimOffset_Yaw) < TurnInPlaceEndAngle)
		{
			TurnInPlaceType = ETurnInPlace::ETIP_NotTurning;
			AimOffset_StartYaw = GetRemoveControlYaw();
		}
	}
}

float UIHPlayerFireComponent::GetRemoveControlYaw()
{
	if (!OwnerCharacter->IsLocallyControlled())
	{
		return FRotator::NormalizeAxis(OwnerCharacter->GetActorRotation().Yaw);
	}

	return OwnerCharacter->GetControlRotation().Yaw;
}

void UIHPlayerFireComponent::UpdtaeCrosshairSpread(float DeltaTime)
{

	APlayerController* playercontroller = Cast<APlayerController>(OwnerCharacter->GetController());

	if (Weapon == nullptr)
		return;

	AIHHUD* HUD = Cast<AIHHUD>(playercontroller->GetHUD());
	if(HUD == nullptr)
		return;

	//이동에 의한 
	FVector2D WalkSpreadrange(0.0f, OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityMulRange(0.0f, 1.0f);
	FVector Velocity = OwnerCharacter->GetVelocity();
	Velocity.Z = 0;

	float CrosshairVelocityAlpha = FMath::GetMappedRangeValueClamped(WalkSpreadrange, VelocityMulRange, Velocity.Size());

	float CrosshairVelocityFactor = FMath::Lerp(Weapon->CrosshairSpreadMin, Weapon->CrosshairSpreadMAx, CrosshairVelocityAlpha);

	TotalCrosshairFactor = CrosshairVelocityFactor + CrosshairFireFactor;

	CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, 0, DeltaTime, 5.0f);

	/////////////
	HUD->SetCrosshairSpread(TotalCrosshairFactor);
}

FVector UIHPlayerFireComponent::GetRandomAimDirection()
{
	if (TotalCrosshairFactor > 0)
	{
		FVector RandomDirection =  FMath::VRandCone(CamComp->GetForwardVector(), FMath::DegreesToRadians(TotalCrosshairFactor) * 0.5);
		return RandomDirection.GetSafeNormal();
	}

	return CamComp->GetForwardVector();
}

void UIHPlayerFireComponent::CheckCrosshairTarget()
{
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());

	if (PC == nullptr || Weapon == nullptr)
		return;

	AIHHUD* HUD = Cast<AIHHUD>(PC->GetHUD());
	if (HUD == nullptr)
		return;


	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector AimDir = CamRot.Vector().GetSafeNormal();
	FVector CameraEnd = CamLoc + (AimDir * Weapon->MaxDistance);

	//FVector MuzzleFlashPosition = Weapon->WeaponMesh->GetSocketLocation("MuzzleFlash");
	FVector Weaponloc = Weapon->GetMuzzleFlashLocation();


	FVector Start = CameraEnd + ( (Weaponloc - CameraEnd).Dot(AimDir) * AimDir );
	FVector End = Start + AimDir * Weapon->MaxDistance;

	//@TODO 여기. 이제 컴포넌트로 변경되었으니 부모의 Actor전달
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);	// 자기 자신(플레이어)는 충돌에서 제외
	Params.AddIgnoredActor(Weapon);

	bool bHit = GetWorld()->LineTraceSingleByChannel(CrosshirHitInfo, Start, End,
		ECC_TRACE_WEAPON /*ECC_Visibility*/, Params);

	if (IHConsoleVariables::DrawCrosshairLine)
	{
		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Red,
			false,
			1.0f,
			0.0f,
			0.5f);
	}

	if (bHit)
	{
		AActor* HitActor = CrosshirHitInfo.GetActor();

		ACharacter* HittedCharacter =  Cast<ACharacter>(HitActor);

		if (HittedCharacter)
		{
			HUD->SetCrosshairColor(FColor::Red);
		}

		else
		{
			HUD->SetCrosshairColor(FColor::White);
		}
	}
}

void UIHPlayerFireComponent::ThrowingAnimPlay_Multicast_Implementation()
{
	UIHPlayerAnimInstance* AnimInstance = Cast<UIHPlayerAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayDrawingAnim();
	}
}

void UIHPlayerFireComponent::ThrowingAction_Server2_Implementation()
{
	ThrowingAnimPlay_Multicast();
}

void UIHPlayerFireComponent::ThoringKnife()
{
	SpawnThrowing();
}

void UIHPlayerFireComponent::SpawnThrowing_Implementation()
{
	if (ProjectileClass)
	{
		const auto Character = Cast<AIHPlayer>(GetOwner());
		const auto ProjectileSpawnLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 50;
		const auto ProjectileSpawnRotation = Character->CamComp->GetComponentRotation();

		auto ProjectileSpawnParams = FActorSpawnParameters();
		ProjectileSpawnParams.Owner = GetOwner();
		ProjectileSpawnParams.Instigator = Character;

		GetWorld()->SpawnActor<ATPS_BaseWeaponProjectile>(ProjectileClass, ProjectileSpawnLocation, ProjectileSpawnRotation, ProjectileSpawnParams);
	}
}

//void UIHPlayerFireComponent::ThoringKnife()
//{
//	SpawnThrowing_Server();
//}
//
//void UIHPlayerFireComponent::SpawnThrowing_Server_Implementation()
//{
//	//SpawnThrowing_Multicast();
//
//	if (ProjectileClass)
//	{
//		const auto Character = Cast<AIHPlayer>(GetOwner());
//		const auto ProjectileSpawnLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 50;
//		const auto ProjectileSpawnRotation = Character->CamComp->GetComponentRotation();
//
//		auto ProjectileSpawnParams = FActorSpawnParameters();
//		ProjectileSpawnParams.Owner = GetOwner();
//		ProjectileSpawnParams.Instigator = Character;
//
//		GetWorld()->SpawnActor<ATPS_BaseWeaponProjectile>(ProjectileClass, ProjectileSpawnLocation, ProjectileSpawnRotation, ProjectileSpawnParams);
//	}
//}
//
//void UIHPlayerFireComponent::SpawnThrowing_Multicast_Implementation()
//{
//	if (GetOwner()->GetLocalRole() != ENetRole::ROLE_Authority)
//		return;
//
//	if (ProjectileClass)
//	{
//		const auto Character = Cast<AIHPlayer>(GetOwner());
//		const auto ProjectileSpawnLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 50;
//		const auto ProjectileSpawnRotation = Character->CamComp->GetComponentRotation();
//
//		auto ProjectileSpawnParams = FActorSpawnParameters();
//		ProjectileSpawnParams.Owner = GetOwner();
//		ProjectileSpawnParams.Instigator = Character;
//
//		GetWorld()->SpawnActor<ATPS_BaseWeaponProjectile>(ProjectileClass, ProjectileSpawnLocation, ProjectileSpawnRotation, ProjectileSpawnParams);
//	}
//}
//
//void UIHPlayerFireComponent::ThrowingAction_Server_Implementation()
//{
//	auto Roll = GetOwner()->GetLocalRole();
//
//	ActionThrowing();
//	//ActionDrowing
//}
//
//void UIHPlayerFireComponent::ActionThrowing_Implementation()
//{
//	UIHPlayerAnimInstance* AnimInstance = Cast<UIHPlayerAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
//	if (AnimInstance)
//	{
//		AnimInstance->PlayDrawingAnim();
//	}
//}
//
//void UIHPlayerFireComponent::Throw_Knife_Server_Implementation()
//{
//	UIHPlayerAnimInstance* AnimInstance = Cast<UIHPlayerAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
//	if (AnimInstance)
//	{
//		AnimInstance->PlayDrawingAnim();
//	}
//}

void UIHPlayerFireComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UIHPlayerFireComponent, Weapon);
}

void UIHPlayerFireComponent::CheckCameraVisible()
{
	// 카메라와 캐릭터가 너무 가까우면, 캐릭터 메시를 숨김

	if (OwnerCharacter->IsLocallyControlled() == false)
		return;

	FVector DistVec = CamComp->GetComponentLocation() - OwnerCharacter->GetActorLocation();
	//FVector DistVec = OwnerCharacter->GetComponentByClass(UCameraComponent::StaticClass())->GetOwner()->GetActorLocation() - OwnerCharacter->GetActorLocation();

	float Distance = DistVec.Size();
	if (Distance < CameraThreshold)
	{
		OwnerCharacter->GetMesh()->SetVisibility(false);
		//OwnerCharacter->GetMesh()->SetVisibility(false);
		//OwnerCharacter->GetMesh()->SetOnlyOwnerSee(true);

		if (Weapon)
		{
			Weapon->WeaponMesh->SetOwnerNoSee(true);
		}

		
	}

	else
	{
		OwnerCharacter->GetMesh()->SetVisibility(true);
		//OwnerCharacter->GetMesh()->SetOnlyOwnerSee(false);

		if (Weapon)
		{
			Weapon->WeaponMesh->SetOwnerNoSee(false);
		}

		
	}
}

void UIHPlayerFireComponent::InputFire_Server_Implementation(FVector_NetQuantize10 startPos, FVector_NetQuantize10 randomDirection)
{
	InputFire_Multicast(startPos, randomDirection);
}

void UIHPlayerFireComponent::InputFire_Multicast_Implementation(FVector_NetQuantize10 startPos, FVector_NetQuantize10 randomDirection)
{
	if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())// && !OwnerCharacter->HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("[Recieved Data]  Input Fire : [StartPos] %f  %f , [RandomDir] %f  %f "), startPos.X, startPos.Y, randomDirection.X, randomDirection.Y);

	LocalFire(startPos, randomDirection);
}

void UIHPlayerFireComponent::LocalFire(FVector startPos, FVector randomDirection)
{
	if (Weapon == nullptr)
		return;

	if (OwnerCharacter->IsLocallyControlled())
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(FireCameraShake);
	}
	

	UIHPlayerAnimInstance* AnimInstance = Cast<UIHPlayerAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayAttackAnim(OwnerCharacter->MoveComp->bIsRun);

		APlayerController* playercontroller = Cast<APlayerController>(OwnerCharacter->GetController());

		CrosshairFireFactor = Weapon->CrosshairSpreadFire;
	}

	if (Weapon)
	{
		Weapon->PlayFireAnimation();
	}

	FVector EndPos = CamComp->GetComponentLocation() + randomDirection * Weapon->MaxDistance;

	// LineTrace 의 충돌 정보를 담을 변수
	FHitResult HitInfo;

	//@TODO 여기. 이제 컴포넌트로 변경되었으니 부모의 Actor전달
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);	// 자기 자신(플레이어)는 충돌에서 제외

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, startPos, EndPos,
		ECC_TRACE_WEAPON /*ECC_Visibility*/, Params);

	// 총기마다 사정거리 MaxDistance
	// 충돌이 안되었으면 최대 사정거리까지만 총알이 날아간다.
	if (bHit == false)
	{
		HitInfo.ImpactPoint = startPos + randomDirection * Weapon->MaxDistance;
	}

	// 총알 궤적 파티클이 설정되어있으면
	if (BeamParticelFactory)
	{
		// 무기 메시의 총구 위치를 가져온다.
		FVector firePosition = Weapon->WeaponMesh->GetSocketTransform(TEXT("MuzzlePosition")).GetLocation();
		UNiagaraComponent* BeamParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BeamParticelFactory, firePosition);
		if (BeamParticle)
		{
			// 목표위치 지점을 설정
			// 다이나믹 머티리얼 인스턴스 
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
				BeamParticle,
				FName("ImpactPositions"),  // Niagara 변수 이름
				TArray<FVector>({ HitInfo.ImpactPoint })  // ImpactPoint를 포함하는 배열
			);

			BeamParticle->SetVariableBool(FName(TEXT("Trigger")), true);
		}
	}

	// LineTrace가 부딪혔을 때
	if (bHit)
	{
		// 총알 파티클
		FTransform bulletTrans;
		bulletTrans.SetLocation(HitInfo.ImpactPoint);
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletEffectFactory, bulletTrans);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BulletEffectFactory, HitInfo.ImpactPoint);

		// Hit 사운드
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitInfo.ImpactPoint);

		// 탄흔 흔적 데칼 
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
			BulletDecalMaterial,	// 데칼 머티리얼 자체를 변수로
			DecalSize,	// 사이즈는 원하는 데칼 크기
			HitInfo.ImpactPoint,
			HitInfo.ImpactNormal.Rotation(),
			DecalLifetime);	// 탄흔이 몇초동안 유지되어야 하는지

		Decal->SetFadeScreenSize(0); // 화면 크기에 따른 페이드 설정

		//auto HitComp = HitInfo.GetComponent();
		//// 충돌한 물체가 물리 적용되어 있다면
		//if (HitComp && HitComp->IsSimulatingPhysics())
		//{
		//	FVector Force = randomDirection * HitComp->GetMass() * 500000;

		//	// 충돌체크된 위치에 힘 전달
		//	HitComp->AddForceAtLocation(Force, HitInfo.ImpactPoint);
		//}

		if (OwnerCharacter->HasAuthority())
		{
			bool bIsHeadShot = false;
			if (ACharacter* HitCharacater = Cast<ACharacter>(HitInfo.GetActor()))
			{
				FHitResult BoneHitInfo;
				FCollisionQueryParams BoneHitParams(SCENE_QUERY_STAT(ECC_TRACE_WEAPON));

				if (HitCharacater->GetMesh()->LineTraceComponent(BoneHitInfo, startPos, EndPos, BoneHitParams))
				{
					bIsHeadShot = BoneHitInfo.BoneName.ToString().Contains(TEXT("head"));
				}
			}

			float TotalDamage = Weapon->damage * OwnerCharacter->GetCharacterStat()->DamageMultiplier;

			if (bIsHeadShot)
			{
				TotalDamage = 100;
			}

			UGameplayStatics::ApplyPointDamage(
				HitInfo.GetActor(),
				TotalDamage,
				randomDirection,
				HitInfo,
				OwnerCharacter->GetController(),
				OwnerCharacter,
				UDamageType::StaticClass()
			);

			// 충돌한 대상이 적인지 판단
			if (OwnerCharacter->HasAuthority())
			{
				UObject* Enemy = HitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
				//HitInfo.GetActor()->GetComponentByClass()
				if (Enemy)
				{
					UIHEnemyFSM* FSM = Cast<UIHEnemyFSM>(Enemy);
					if (FSM)
					{
						FSM->OnDamageProcess();
					}
				}
			}
		}

		
		
	}
}