// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TPS_BaseWeaponProjectile.h"
#include "../Character/TPS_Character.h"
#include "../Character/IHPlayer.h"
#include "../CharacterStat/CharacterStat.h"
#include <Components/SphereComponent.h>
#include <Engine/DamageEvents.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ATPS_BaseWeaponProjectile::ATPS_BaseWeaponProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Collision");
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(10.0f);
	SphereCollision->BodyInstance.SetCollisionProfileName("BlockAll");
	SphereCollision->OnComponentHit.AddDynamic(this, &ATPS_BaseWeaponProjectile::OnHit);

	RootComponent = SphereCollision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetRelativeLocation(FVector(-40.0f, 0.0f, 0.0f));
	Mesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> KnifeMesh(TEXT("/Script/Engine.StaticMesh'/Game/Assets/KayKit/DungeonElements/dagger_common.dagger_common'"));

	if (KnifeMesh.Object)
	{
		Mesh->SetStaticMesh(KnifeMesh.Object);
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereCollision;
	ProjectileMovement->ProjectileGravityScale = 1;
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->MaxSpeed = 3000;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	bReplicates = true;

}

void ATPS_BaseWeaponProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto ComputedDamage = Damage;
	if (const auto Character = Cast<AIHPlayer>(GetInstigator()))
	{
		ComputedDamage *= Character->GetCharacterStat()->DamageMultiplier;
	}

	if (OtherActor && OtherActor != this)
	{
		const FDamageEvent Event(UDamageType::StaticClass());
		OtherActor->TakeDamage(ComputedDamage, Event, GetInstigatorController(), this);
	}

	Destroy();
}

// Called when the game starts or when spawned
void ATPS_BaseWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPS_BaseWeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

