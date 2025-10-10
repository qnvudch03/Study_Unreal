// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnner.generated.h"

UCLASS()
class MYIHGAME_API AEnemySpawnner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	FTimerHandle spawnTimerHandle;

	float SpawnInterval = 5;
	TArray<class AIHEnemySpawnPoint*> spawnPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|MinionsList", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class ATPS_Minion>> MinionList;

	void SpawnEnemy();
};
