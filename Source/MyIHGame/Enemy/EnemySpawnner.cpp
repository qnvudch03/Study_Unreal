// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemySpawnner.h"
#include <Kismet/GameplayStatics.h>
#include "IHEnemySpawnPoint.h"
#include "TPS_Minion.h"
#include "IHEnemy.h"

// Sets default values
AEnemySpawnner::AEnemySpawnner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemySpawnner::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	TArray<AActor*> Points;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIHEnemySpawnPoint::StaticClass(), Points);

	if (Points.Num() != 0)
	{
		for (auto point : Points)
		{
			spawnPoints.Add(static_cast<AIHEnemySpawnPoint*>(point));
		}

		GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemySpawnner::SpawnEnemy, SpawnInterval);
	}
	

}

// Called every frame
void AEnemySpawnner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawnner::SpawnEnemy()
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	int randomPositionIndex = FMath::RandRange(0, spawnPoints.Num() - 1);
	int randomMinionIndex = FMath::RandRange(0, MinionList.Num() - 1);

	FVector spanwedPosition = spawnPoints[randomPositionIndex]->GetActorLocation();

	if (MinionList.Num() == 0)
	{
		GetWorld()->SpawnActor<ATPS_Minion>(ATPS_Minion::StaticClass(), spanwedPosition, FRotator::ZeroRotator);
	}

	else
	{
		GetWorld()->SpawnActor<ATPS_Minion>(MinionList[randomMinionIndex], spanwedPosition, FRotator::ZeroRotator);
	}
	
	GetWorld()->SpawnActor<AIHEnemy>(AIHEnemy::StaticClass(), spanwedPosition, FRotator::ZeroRotator);
	

	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemySpawnner::SpawnEnemy, SpawnInterval);
}

