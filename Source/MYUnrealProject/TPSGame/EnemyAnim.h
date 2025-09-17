// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class MYUNREALPROJECT_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WalkingMaxSpeed = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RunningMaxSpeed = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EEnemyState EnemyState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAttackPlay = false;

	UFUNCTION(BlueprintCallable)
	void OnEndAttackAnimation();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDamageAnim(FName SectionName);

	UFUNCTION(BlueprintCallable)
	void OnDieAnimEnd();

	
};
