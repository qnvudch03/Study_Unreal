// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHEnemyManager.h"
#include "IHEnemy.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "IHEnemySpawnPoint.h"

// Sets default values
AIHEnemyManager::AIHEnemyManager()
{
 	// Tick 에서 구현할 내용이 없다.
	// 타이머를 활용할 예정
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AIHEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	float CreateTime = FMath::RandRange(MinTime, MaxTime);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AIHEnemyManager::CreateEnemy, CreateTime);

	// 스폰 위치 동적 할당
	FindSpawnPoints();
}


void AIHEnemyManager::CreateEnemy()
{
	// 랜덤 위치 구하기
	int index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	
	// 적 생성 및 배치하기
	GetWorld()->SpawnActor<AIHEnemy>(EnemyFactory, SpawnPoints[index]->GetActorLocation(), FRotator(0));

	// 다시 랜덤 시간에 CreateEnemy 함수가 호출되도록 타이머 설정
	float CreateTime = FMath::RandRange(MinTime, MaxTime);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AIHEnemyManager::CreateEnemy, CreateTime);
}

void AIHEnemyManager::FindSpawnPoints()
{
	// 원하는 타입의 액터 모두 찾아오기
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), 
		AIHEnemySpawnPoint::StaticClass() /*AActor::StaticClass()*/, AllActors);

	// SpanwPoint 만 순회한다.
	for (auto spawn : AllActors)
	{
		// 스폰 목록에 추가
		SpawnPoints.Add(spawn);
	}
}
