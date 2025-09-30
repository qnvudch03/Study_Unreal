// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHEnemySpawnPoint.generated.h"

UCLASS()
class MYIHGAME_API AIHEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHEnemySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 에디터용 아이콘
	//UPROPERTY()
	//class UMaterialBillobardComponent* Icon;

	// 스폰되어야하는 Enemy 타입도 지정

};
