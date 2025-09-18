// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class MYUNREALPROJECT_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

	UPROPERTY(EditAnywhere, Category = "MY|SpawnMinTime")
	float SpawnMinTime = 3;

	UPROPERTY(EditAnywhere, Category = "MY|SpawnMaxTime")
	float SpawnMaxTime = 8;

	UPROPERTY(EditAnywhere, Category = "MY|SpawnPos")
	TArray<class AEnemySpawnPoint*> SpawnPoses;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Enemy")
	TSubclassOf<class AEnemy> Enemy;

	FTimerHandle spawnTimerHandle;

	void CreateEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
