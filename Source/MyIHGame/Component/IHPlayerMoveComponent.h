// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/IHPlayerBaseComponent.h"
#include "IHPlayerMoveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedChange, float, NewSpeed);

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

	void SprintStart(const struct FInputActionValue& InputValue);
	void SprintEnd(const struct FInputActionValue& InputValue);

	UPROPERTY()
	AActor* InteractableActor;

	UPROPERTY()
	class AIHColorBox* CurrentHoldColorBox;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PickDropColorBox(class AIHColorBox* OldBox, class AIHColorBox* NewBox);

	virtual void Multicast_PickDropColorBox_Implementation(class AIHColorBox* OldBox, class AIHColorBox* NewBox);
};
