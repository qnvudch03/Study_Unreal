// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy/IHEnemyFSM.h"
#include "IHEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IHGame|FSM")
	EEnemyState AnimState;

	// 공격 상태 재생할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|FSM")
	bool bAttackPlay = false;

	// 공격 끝나면 재생되는 함수
	UFUNCTION(BlueprintCallable, Category = "IHGame|Value")
	void OnEndAttackAnimation();

	// 피격 애니메이션 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "IHGame|Value")
	void PlayDamageAnim(FName SectionName);
	
	// 죽음 상태 애니메이션 종료 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool bDieDone = false;

	// 테스트용 HIT
	UFUNCTION(BlueprintCallable)
	void TEST_OnHitPlayer();
};
