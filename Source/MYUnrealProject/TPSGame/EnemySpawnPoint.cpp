// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/EnemySpawnPoint.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UStaticMesh> circleMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (circleMesh.Object)
	{
		bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));

		bodyMesh->SetStaticMesh(circleMesh.Object);
		bodyMesh->SetCollisionProfileName("NoCollision");
		bodyMesh->SetWorldScale3D(FVector(0.5, 0.5, 0.5));

	}

}

// Called when the game starts or when spawned
void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	bodyMesh->SetVisibility(false);
}

