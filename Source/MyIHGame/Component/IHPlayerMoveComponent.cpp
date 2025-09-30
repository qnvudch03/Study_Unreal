// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/IHPlayerMoveComponent.h"
#include "Character/IHPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/IHDataSubsystem.h"
#include "Data/IHInputDataAsset.h"
#include "IHGameplayTags.h"

UIHPlayerMoveComponent::UIHPlayerMoveComponent()
{
	// Tick 함수 호출 필요없음.
	PrimaryComponentTick.bCanEverTick = false;
}

void UIHPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기 속도를 걷기로 설정
	CharacterMoveComp->MaxWalkSpeed = WalkSpeed;
}

void UIHPlayerMoveComponent::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	// 싱글톤 GameInstance 가져오기
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UIHDataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UIHDataSubsystem>();
	if (DataSubsystem == nullptr)
		return;

	UIHInputDataAsset* InputData = DataSubsystem->InputDataAsset;
	if (InputData == nullptr)
		return;

	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Look), ETriggerEvent::Triggered, this, &UIHPlayerMoveComponent::Look);
	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Move), ETriggerEvent::Triggered, this, &UIHPlayerMoveComponent::Move);


	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Run), ETriggerEvent::Started, this, &UIHPlayerMoveComponent::InputRun);
	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Run), ETriggerEvent::Completed, this, &UIHPlayerMoveComponent::InputRun);

	// 점프 입력 이벤트 처리 함수 바인딩
	PlayerInput->BindAction(InputData->FindInputActionByTag(IHGameplayTags::Input_Action_Jump), ETriggerEvent::Started, this, &UIHPlayerMoveComponent::InputJump);
}

void UIHPlayerMoveComponent::Look(const FInputActionValue& InputValue)
{
	// 삼인칭 템플릿 기준 2D 축으로 변경
	const FVector2D Value = InputValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		OwnerCharacter->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		OwnerCharacter->AddControllerPitchInput(Value.Y);
	}
}

void UIHPlayerMoveComponent::Move(const FInputActionValue& InputValue)
{
	const FVector2D Value = InputValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, OwnerCharacter->Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		OwnerCharacter->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		OwnerCharacter->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UIHPlayerMoveComponent::InputJump(const FInputActionValue& InputValue)
{
	OwnerCharacter->Jump();
}


void UIHPlayerMoveComponent::InputRun(const FInputActionValue& InputValue)
{
	UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
	// 현재 달리기 모드라면
	if (Movement->MaxWalkSpeed > WalkSpeed)
	{
		// 걷기 속도로 전환
		Movement->MaxWalkSpeed = WalkSpeed;
		bIsRun = false;
	}
	else
	{
		Movement->MaxWalkSpeed = RunSpeed;
		bIsRun = true;
	}
}

void UIHPlayerMoveComponent::InputInteract(const FInputActionValue& InputValue)
{
}
