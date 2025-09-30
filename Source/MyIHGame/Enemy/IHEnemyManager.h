// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHEnemyManager.generated.h"

UCLASS()
class MYIHGAME_API AIHEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// 랜덤 스폰 시간
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	float MinTime = 1;

	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	float MaxTime = 5;

	// 스폰할 위치
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TArray<class AActor*> SpawnPoints;

	// 스폰할 Enemy 블루프린트 타입
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class AIHEnemy> EnemyFactory;

	// 스폰을 위한 알람 타이머
	FTimerHandle SpawnTimerHandle;

	// 적 생성 함수
	void CreateEnemy();

	// 스폰할 위치 동적 찾아 할당하기
	void FindSpawnPoints();
};
