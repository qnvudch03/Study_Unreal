// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IHPlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYIHGAME_API UIHPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIHPlayerBaseComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

public:
	// 사용자 입력 맵핑 처리 함수
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) {};

	// 컴포넌트 소유 액터
	UPROPERTY()
	class AIHPlayer* OwnerCharacter;

	UPROPERTY()
	class UCharacterMovementComponent* CharacterMoveComp;
};
