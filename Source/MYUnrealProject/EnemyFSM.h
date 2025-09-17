// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle, Move,
	Attack,
	Damage,
	Die,
	State_Max
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYUNREALPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "FSM|State")
	EEnemyState State = EEnemyState::Idle;

	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DeadState();

public:
	float CurrentIdleTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FSM|IdleTime")
	float MaxIdleTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FSM|AttackInteval")
	float AttackDuration = 2.0f;
	float AttackTimer = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FSM|AttackRange")
	float AttackRange = 150.0f;


public:
	UPROPERTY(EditDefaultsOnly, Category = "FSM|Target")
	TObjectPtr<class ATPSPlayer> Target;

	UPROPERTY(EditDefaultsOnly, Category = "FSM|Me")
	TObjectPtr<class AEnemy> Me;

	UPROPERTY(EditAnywhere, Category = "FSM|HP")
	int32 HP = 4;

	UPROPERTY(EditAnywhere, Category = "FSM|HitDelay")
	float DamageDelayTime = 2.0f;

	float CurrentDamageDelayTime = 0.0f;
	void onAttackAnimEnd();

	void OnDamagedProcess();

	UPROPERTY()
	class UEnemyAnim* EnemyAnim;

	bool bIsAlive = true;

	void OnDie();
		
};
