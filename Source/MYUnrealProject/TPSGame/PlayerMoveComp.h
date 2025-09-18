// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSGame/PlayerBaseComponent.h"
#include "PlayerMoveComp.generated.h"

/**
 * 
 */
UCLASS()
class MYUNREALPROJECT_API UPlayerMoveComp : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:

	void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "MY|Input")
	class UInputAction* ActionLook = nullptr;

	UPROPERTY(EditAnywhere, Category = "MY|Input")
	class UInputAction* ActionJump = nullptr;

	UPROPERTY(EditAnywhere, Category = "MY|Input")
	class UInputAction* ActionMove = nullptr;

	void Look(const struct FInputActionValue& inputValue);
	void CheckCameraVisible();

	FVector Direction;
	//float MovingSpeed = 100;
	void Move(const struct FInputActionValue& inputValue);
	void Moving();

	void InputJump(const struct FInputActionValue& inputValue);

public:
	void SetupInputBinding(class UEnhancedInputComponent* playerInput) override;

private:
	float CameraMinDistance = 35;

	UPROPERTY(VisibleAnywhere, Category = "MY|FollowCamera")
	class UCameraComponent* FollowCamera = nullptr;
	
};
