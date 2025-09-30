// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/IHPlayerBaseComponent.h"
#include "IHPlayerMoveComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHPlayerMoveComponent : public UIHPlayerBaseComponent
{
	GENERATED_BODY()
	
public:
	UIHPlayerMoveComponent();
	virtual void BeginPlay() override;
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;

public:

	// 이동 속도
	UPROPERTY(EditAnywhere, Category = "IHGame|Value")
	float WalkSpeed = 200;
	// 달리기 속도
	UPROPERTY(EditAnywhere, Category = "IHGame|Value")
	float RunSpeed = 600;

	// 달리기중인지
	bool bIsRun = false;

	// 카메라 회전 입력 처리
	void Look(const struct FInputActionValue& InputValue);
	// 이동 입력 처리
	void Move(const struct FInputActionValue& InputValue);
	// 점프 입력 이벤트 처리 함수
	void InputJump(const struct FInputActionValue& InputValue);
	// 달리기
	void InputRun(const struct FInputActionValue& InputValue);
	void InputInteract(const struct FInputActionValue& InputValue);
};
