// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/Bullet.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	collision->SetCollisionProfileName(TEXT("BlockAll"));
	collision->SetCapsuleHalfHeight(16);
	collision->SetCapsuleRadius(8);

	collision->SetupAttachment(RootComponent);

	//RootComponent = collision;

	ConstructorHelpers::FObjectFinder<UStaticMesh> bulletMesh(TEXT("/Script/Engine.StaticMesh'/Game/TPSGame/Bullet/9mm_luger_fbx.9mm_luger_fbx'"));

	if (bulletMesh.Object)
	{
		BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
		BulletMesh->SetStaticMesh(bulletMesh.Object);
		BulletMesh->SetupAttachment(collision);
		BulletMesh->SetCollisionProfileName(TEXT("NoCollision"));
		BulletMesh->SetRelativeRotation(FRotator(0, 0, 90));
		BulletMesh->SetRelativeLocation(FVector(0, -55, 15));
		BulletMesh->SetRelativeScale3D(FVector(10));
		//BulletMesh->SetRelativeScale3D(FVector(0.25f));
	}

	
	{
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
		ProjectileMovement->SetUpdatedComponent(collision);
		ProjectileMovement->InitialSpeed = 5000;
		ProjectileMovement->MaxSpeed = 5000;
		ProjectileMovement->bShouldBounce = true;
		ProjectileMovement->Bounciness = 0.3f;
	}
	
	

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle deathTimer;
	TWeakObjectPtr<ABullet> WeakPtr = this;

	GetWorld()->GetTimerManager().SetTimer(deathTimer,
		FTimerDelegate::CreateLambda([WeakPtr]()
			{
				if (WeakPtr.IsValid())
				{
					WeakPtr->Destroy();
				}
			}
		)
		, 2.0f, false);

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

