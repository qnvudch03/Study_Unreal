// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHEnemySpawnPoint.h"
//#include "Components/MaterialBillobardComponent.h"

// Sets default values
AIHEnemySpawnPoint::AIHEnemySpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Icon = CreateDefaultSubobject<UMaterialBillobardComponent>(TEXT("Icon"));
}

// Called when the game starts or when spawned
void AIHEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIHEnemySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

