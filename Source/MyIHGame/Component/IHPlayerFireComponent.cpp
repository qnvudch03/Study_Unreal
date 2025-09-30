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

UIHPlayerFireComponent::UIHPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UIHPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	CamComp = OwnerCharacter->CamComp;

	// 스타팅 무기를 월드에 진짜로 스폰시킨다.
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

void UIHPlayerFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 카메라가 캐릭터와 너무 가까우면 캐릭터를 숨김처리
	CheckCameraVisible();

	// 에임 오프셋
	UpdateAimOffset(DeltaTime);
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

	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Fire), ETriggerEvent::Started, this, &UIHPlayerFireComponent::InputFire);
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
		float CurrentAimYaw = OwnerCharacter->GetControlRotation().Yaw;
		float DeltaYaw = FMath::FindDeltaAngleDegrees(AimOffset_StartYaw, CurrentAimYaw);
		// -180~180
		AimOffset_Yaw = DeltaYaw;

		// 일시적으로 카메라=캐릭터의 회전 싱크를 끊는다.
		OwnerCharacter->bUseControllerRotationYaw = false;

		PRINT_LOG(TEXT("AO_StartYaw : %f, AO_Yaw : %f"), AimOffset_StartYaw, AimOffset_Yaw);
	}
	// 이동중일떄는 카메라 적용
	if (Speed > 0 || bIsInAir)
	{
		// 이동중일때는 카메라의 회전값 = AimOffset의 기준값
		AimOffset_StartYaw = OwnerCharacter->GetControlRotation().Yaw;
		AimOffset_Yaw = 0;	// Yaw값은 가만히 서있을때만 갱신된다.

		OwnerCharacter->bUseControllerRotationYaw = true;
	}

	// Pitch
	AimOffset_Pitch = OwnerCharacter->GetBaseAimRotation().Pitch;
}

void UIHPlayerFireComponent::InputFire(const FInputActionValue& InputValue)
{
	if (!OwnerCharacter)
	{
		return;
	}

	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(FireCameraShake);

	UIHPlayerAnimInstance* AnimInstance = Cast<UIHPlayerAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		//@TODO
		// Move Component에서 가져온다.
		AnimInstance->PlayAttackAnim(OwnerCharacter->MoveComp->bIsRun);
	}

	{
		FVector StartPos = CamComp->GetComponentLocation();
		FVector EndPos = CamComp->GetComponentLocation() + CamComp->GetForwardVector() * 5000;
		FVector Dir = EndPos - StartPos;
		Dir.Normalize();

		// LineTrace 의 충돌 정보를 담을 변수
		FHitResult HitInfo;

		//@TODO 여기. 이제 컴포넌트로 변경되었으니 부모의 Actor전달
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerCharacter);	// 자기 자신(플레이어)는 충돌에서 제외

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPos, EndPos,
			ECC_TRACE_WEAPON /*ECC_Visibility*/, Params);

		// 총기마다 사정거리 MaxDistance
		// 충돌이 안되었으면 최대 사정거리까지만 총알이 날아간다.
		if (bHit == false)
		{
			HitInfo.ImpactPoint = StartPos + Dir * Weapon->MaxDistance;
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

			auto HitComp = HitInfo.GetComponent();
			// 충돌한 물체가 물리 적용되어 있다면
			if (HitComp && HitComp->IsSimulatingPhysics())
			{
				FVector Force = Dir * HitComp->GetMass() * 500000;

				// 충돌체크된 위치에 힘 전달
				HitComp->AddForceAtLocation(Force, HitInfo.ImpactPoint);
			}

			// 충돌한 대상이 적인지 판단
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
		Weapon->WeaponMesh->SetOwnerNoSee(true);
	}

	else
	{
		OwnerCharacter->GetMesh()->SetVisibility(true);
		//OwnerCharacter->GetMesh()->SetOnlyOwnerSee(false);
		Weapon->WeaponMesh->SetOwnerNoSee(false);
	}
}