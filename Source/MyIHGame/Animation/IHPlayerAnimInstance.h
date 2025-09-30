// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "IHPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	// 플레이어 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	float Speed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	float Direction = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	bool bIsInAir = false;

	// 입력이 있는 상태로 이동중인지 판단은 velocity 말고 accel(가속도)값을 사용하자.
	UPROPERTY(BlueprintReadWrite, Category = "IHGame|Value")
	bool bIsAccelerating = false;

	// C++ 클래스에 Velocity변수를 추가하고 연동한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	FVector Velocity;

	// 재생할 공격 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "IHGame|Anim")
	class UAnimMontage* AttackAnimMontage;

	// 에임 오프셋값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	float AO_Yaw = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	float AO_Pitch = 0;

	// 왼손이 있어야하는 Transform 을 역으로 계산
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	FTransform LeftHandTransfrom;

	// 공격 애니메이션 재생 함수
	void PlayAttackAnim(bool bIsRun);
};
