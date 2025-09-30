// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IHEnemy.generated.h"

UCLASS()
class MYIHGAME_API AIHEnemy : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame|FSM")
	class UIHEnemyFSM* FSM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IHGame|FSM")
	class UNavigationInvokerComponent* NavInvoker;
};
