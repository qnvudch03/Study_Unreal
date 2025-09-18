// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/PlayerMoveComp.h"
#include <InputActionValue.h>
#include <Camera/CameraComponent.h>
#include <EnhancedInputComponent.h>

void UPlayerMoveComp::SetupInputBinding(UEnhancedInputComponent* playerInput)
{
	playerInput->BindAction(ActionLook, ETriggerEvent::Triggered, this, &UPlayerMoveComp::Look);
	playerInput->BindAction(ActionMove, ETriggerEvent::Triggered, this, &UPlayerMoveComp::Move);
	playerInput->BindAction(ActionJump, ETriggerEvent::Triggered, this, &UPlayerMoveComp::InputJump);
}

void UPlayerMoveComp::BeginPlay()
{
	Super::BeginPlay();

	FollowCamera = Me->GetComponentByClass<UCameraComponent>();
}

void UPlayerMoveComp::Look(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	Me->AddControllerYawInput(value.X);

	FRotator CurrentRot = FollowCamera->GetRelativeRotation();
	CurrentRot.Pitch += value.Y;

	if (CurrentRot.Pitch > 25)
	{
		CurrentRot.Pitch = 25;
	}

	else if (CurrentRot.Pitch < -15)
	{
		CurrentRot.Pitch = -15;
	}

	FollowCamera->SetRelativeRotation(CurrentRot);

	CheckCameraVisible();

}

void UPlayerMoveComp::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	Direction.X = value.X;
	Direction.Y = value.Y;

	Moving();
}

void UPlayerMoveComp::Moving()
{
	FRotator Temp1 = Me->GetControlRotation();
	FTransform Temp2 = FTransform(Me->GetControlRotation());
	FVector Temp3 = FTransform(Me->GetControlRotation()).TransformVector(Direction);


	Direction = FTransform(Me->GetControlRotation()).TransformVector(Direction);
	Me->AddMovementInput(Direction);
	Direction = FVector::ZeroVector;
}

void UPlayerMoveComp::InputJump(const FInputActionValue& inputValue)
{
	Me->Jump();
}

void UPlayerMoveComp::CheckCameraVisible()
{
	FVector DistVec = FollowCamera->GetComponentLocation() - Me->GetActorLocation();
	float Distance = DistVec.Size();
	if (Distance < CameraMinDistance)
	{
		Me->GetMesh()->SetVisibility(false);
	}
	else
	{
		Me->GetMesh()->SetVisibility(true);
	}
}
