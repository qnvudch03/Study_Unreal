// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/IHWeapon.h"
#include "../Character/IHPlayer.h"
#include "../Component/IHPlayerFireComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Components/DecalComponent.h>
#include "../Enemy/IHEnemyFSM.h"
#include "../Weapon/IHBullet.h"
#include "Components/SphereComponent.h"

// Sets default values
AIHWeapon::AIHWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));

	// 시작 무기는 가지고 태어날거기때문에 충돌체를 끄고 시작
	CollisionComp->SetCollisionProfileName(TEXT("NoCollision"));
	CollisionComp->SetSphereRadius(64);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 충돌을 루트로 설정
	RootComponent = CollisionComp;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(CollisionComp);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AIHWeapon::BeginPlay()
{
	Super::BeginPlay();

	//CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIHWeapon::OnWeaponOverlap);
	OnActorBeginOverlap.AddDynamic(this, &AIHWeapon::Test);
}

// Called every frame
void AIHWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIHWeapon::PlayFireAnimation()
{
	if (weaponFireAnimation)
	{
		WeaponMesh->PlayAnimation(weaponFireAnimation, false);
	}
}

FVector AIHWeapon::GetMuzzleFlashLocation()
{
	if (WeaponMesh)
	{
		FVector MuzzleFlashLocation = WeaponMesh->GetSocketLocation("MuzzleFlash");
		return MuzzleFlashLocation;
	}

	else
	{
		return FVector::Zero();
	}

}

void AIHWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	AIHPlayer* player = Cast<AIHPlayer>(OtherActor);

	if (!player)
		return;

	if (player->FireComp == nullptr)
		return;

	player->FireComp->ChangeWeapon(this);
}

void AIHWeapon::Test(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!HasAuthority())
		return;

	AIHPlayer* player = Cast<AIHPlayer>(OtherActor);

	if (!player)
		return;

	if (player->FireComp == nullptr)
		return;

	if (player->IsLocallyControlled())
	{
		player->FireComp->ChangeWeapon(this);
	}


}

void AIHWeapon::Equip()
{
	if (GetOwner() == nullptr)
		return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
		return;

	this->SetOwner(Character);

	// 무기가 캐릭터 오른손을 따라다니게 설정한다.
	// 여러분은 hand_rSocket
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("hand_r_socket"));
	if (HandSocket)
	{
		//HandSocket->AttachActor(this, Character->GetMesh());
	}

	this->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AIHWeapon::UnEquip()
{
	if (GetOwner() == nullptr)
		return;

	FVector OwnerLocation = GetOwner()->GetActorLocation();

	FDetachmentTransformRules DetachRule(EDetachmentRule::KeepWorld, true);
	this->DetachFromActor(DetachRule);

	this->SetOwner(nullptr);

	this->SetActorLocation(OwnerLocation);
	this->SetActorRotation(FRotator::ZeroRotator);

	FTimerHandle weaponChangeHandle;

	TWeakObjectPtr<AIHWeapon> pastWeapon = this;
	GetWorld()->GetTimerManager().SetTimer(weaponChangeHandle, FTimerDelegate::CreateLambda([pastWeapon]()
		{
			if (pastWeapon.IsValid())
			{
				pastWeapon->CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
	), 2.0f, false);

}

void AIHWeapon::Fire(FVector startPos, FVector randomDir)
{
	if (Bullet)
	{
		FireBullet(startPos, randomDir);
	}
	else
	{
		FireLineTrace(startPos, randomDir);
	}
}

void AIHWeapon::FireBullet(FVector startPos, FVector randomDir)
{
	if (GetOwner()->HasAuthority() == false)
		return;

	FVector MuzzleFlashLocation = GetMuzzleFlashLocation();



	FActorSpawnParameters SpawnParams;

	SpawnParams.Owner = this;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	AIHBullet* SpawnedBullet =  GetWorld()->SpawnActor<AIHBullet>(Bullet, MuzzleFlashLocation, randomDir.Rotation(), SpawnParams);
	
	SpawnedBullet->CollisionComp->IgnoreActorWhenMoving(Cast<APawn>(GetOwner()), true);
	SpawnedBullet->CollisionComp->IgnoreActorWhenMoving(this, true);

}

void AIHWeapon::FireLineTrace(FVector startPos, FVector randomDir)
{
	AIHPlayer* OwnerCharacter = Cast<AIHPlayer>(GetOwner());
	if (!OwnerCharacter)
		return;

	FVector EndPos = startPos + randomDir * MaxDistance;

	// LineTrace 의 충돌 정보를 담을 변수
	FHitResult HitInfo;

	//@TODO 여기. 이제 컴포넌트로 변경되었으니 부모의 Actor전달
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);	// 자기 자신(플레이어)는 충돌에서 제외
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, startPos, EndPos,
		ECC_TRACE_WEAPON /*ECC_Visibility*/, Params);

	// 총기마다 사정거리 MaxDistance
	// 충돌이 안되었으면 최대 사정거리까지만 총알이 날아간다.
	if (bHit == false)
	{
		HitInfo.ImpactPoint = startPos + randomDir * MaxDistance;
	}

	// 총알 궤적 파티클이 설정되어있으면
	if (BeamParticelFactory)
	{
		// 무기 메시의 총구 위치를 가져온다.
		FVector firePosition = WeaponMesh->GetSocketTransform(TEXT("MuzzlePosition")).GetLocation();
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

			float TotalDamage = damage * OwnerCharacter->GetCharacterStat()->DamageMultiplier;

			if (bIsHeadShot)
			{
				TotalDamage = 100;
			}

			UGameplayStatics::ApplyPointDamage(
				HitInfo.GetActor(),
				TotalDamage,
				randomDir,
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

