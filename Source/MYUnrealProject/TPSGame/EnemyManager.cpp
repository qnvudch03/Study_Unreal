// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/EnemyManager.h"
#include "Enemy.h"
#include "EnemySpawnPoint.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AEnemyManager::CreateEnemy()
{
	int randomSpawnIndex = FMath::RandRange(0, SpawnPoses.Num() - 1);

	GetWorld()->SpawnActor<AEnemy>(Enemy, SpawnPoses[randomSpawnIndex]->GetTransform());

	float randomSpawnTime = FMath::RandRange(SpawnMinTime, SpawnMaxTime);


	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, randomSpawnTime);
	//GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, FTimerDelegate::CreateLambda([this]() {}), 2, false);

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	float randomTime = FMath::RandRange(SpawnMinTime, SpawnMaxTime);

	TArray<AActor*> posActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawnPoint::StaticClass(), posActors);

	for (AActor* positionActor : posActors)
	{
		AEnemySpawnPoint* spawnPoint = CastChecked<AEnemySpawnPoint>(positionActor);

		if (spawnPoint)
		{
			SpawnPoses.Add(spawnPoint);
		}
		
	}

	if (SpawnPoses.Num() != 0)
	{
		GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, randomTime);
	}
	
	
}


