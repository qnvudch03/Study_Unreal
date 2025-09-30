// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/IHPlayerBaseComponent.h"
#include "Character/IHPlayer.h"

// Sets default values for this component's properties
UIHPlayerBaseComponent::UIHPlayerBaseComponent()
{
	bWantsInitializeComponent = true;
}

void UIHPlayerBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();
	OwnerCharacter = Cast<AIHPlayer>(GetOwner());
	CharacterMoveComp = OwnerCharacter->GetCharacterMovement();

	// 델리게이트에 처리 함수 등록
	OwnerCharacter->OnInputBindingDelegate.AddUObject(this, &UIHPlayerBaseComponent::SetupInputBinding);
}

// Called when the game starts
void UIHPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

