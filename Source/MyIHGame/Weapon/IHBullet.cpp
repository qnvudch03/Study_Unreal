// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/IHBullet.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/GameplayStatics.h>
#include "../Weapon/IHWeapon.h"
#include <Particles/ParticleSystem.h>
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AIHBullet::AIHBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 충돌체 등록하기
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	//CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	CollisionComp->SetSphereRadius(13);
	RootComponent = CollisionComp;

	// 2. 외관 컴포넌트 등록하기
	BodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	BodyMeshComp->SetupAttachment(CollisionComp);
	// 루트로 지정된 SphereComponent가 충돌체크 담당이라, Mesh의 충돌은 끈다
	BodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	//BodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// 3. 발사체 컴포넌트
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->SetUpdatedComponent(CollisionComp);
	MovementComp->InitialSpeed = 5000;// 초기 속도
	MovementComp->MaxSpeed = 5000;	// 최대 속도
	MovementComp->bShouldBounce = true;	// 반동 여부
	MovementComp->Bounciness = 0.3f;	// 반동 값

	bReplicates = true;

	// 생명 시간 주기
	//InitialLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void AIHBullet::BeginPlay()
{
	Super::BeginPlay();

	if (TrailEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached
		(
			TrailEffect,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);

		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIHBullet::OnBulletOverlapWith);
		CollisionComp->OnComponentHit.AddDynamic(this, &AIHBullet::OnBulletHitted);
	}
	
	//// 책에서 알려준 람다로 실행되는 타이머
	//// C++ 수업할떄 람다로 this를 캡쳐하고 싶다면?
	//// 보정장치가 필요하다.
	//// this가 항상 유효한지 판단할수 있는 근거가 있어야 된다.
	// 
	//// 방법 1번)
	//// 스마트 포인터를 사용해서 Bullet이 살아있다는 확인을 받고난뒤 Destroy 호출
	//TWeakObjectPtr<AIHBullet> WeakPtr(this);

	////FTimerHandle DeathTimer; 지역변수가 아니라
	//GetWorld()->GetTimerManager().SetTimer(DeathTimer,
	//	FTimerDelegate::CreateLambda([WeakPtr]()->void
	//		{
	//			// 2초뒤에 여기 로직이 실행
	//			if (WeakPtr.IsValid())
	//			{
	//				// this가 살아있다는 전제하에 Destroy()
	//				WeakPtr->Destroy();
	//			}
	//		}), 2.0f, false);

	//// 방법 2번)
	//// Destroy 가되는 순간, 타이머를 취소시킨다.
}

void AIHBullet::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AIHBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIHBullet::Destroyed()
{
	Super::Destroyed();

	// 하고싶은 로직
	// 타이머 정리 - World가 여전히 유효함
	//GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
}

void AIHBullet::Die()
{
	Destroy();
}

#if WITH_EDITOR
void AIHBullet::OnBulletOverlapWith(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
void AIHBullet::OnBulletHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	AIHWeapon* weapon = Cast<AIHWeapon>(GetOwner());
	if (weapon == nullptr)
		return;

	APawn* instigator = Cast<APawn>(GetInstigator());
	if (instigator == nullptr)
		return;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(instigator);

	UGameplayStatics::ApplyRadialDamageWithFalloff
	(
		this,
		weapon->damage,
		weapon->damage_Min,
		Hit.Location,
		weapon->damage_InnerRadious,
		weapon->damage_OuterRadious,
		1.0f,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		instigator->GetController()
	);

	DrawDebugSphere(GetWorld(), Hit.Location, weapon->damage_InnerRadious, 10, FColor::Red, false, 2.0f);
	DrawDebugSphere(GetWorld(), Hit.Location, weapon->damage_OuterRadious, 15, FColor::Blue, false, 2.0f);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MovementComp->StopMovementImmediately();


	if (HittedEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(),
			HittedEffect,
			Hit.Location,
			FRotator(0, FMath::FRandRange(0.f, 360.f), 0),
			true
		);

	}

	SetLifeSpan(1.0f);
}
void AIHBullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// speed 값이 수정되었는지 체크
	if (PropertyChangedEvent.GetPropertyName() == TEXT("Speed"))
	{
		// 프로젝타일 무브먼트 컴포넌트에 speed 값 적용
		MovementComp->InitialSpeed = Speed;
		MovementComp->MaxSpeed = Speed;
	}
}
#endif
