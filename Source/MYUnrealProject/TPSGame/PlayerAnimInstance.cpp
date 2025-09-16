// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/PlayerAnimInstance.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();

	auto player = Cast<ATPSPlayer>(ownerPawn);

	if (player)
	{
		FVector velocity = player->GetVelocity();
		FVector forwordVec = player->GetActorForwardVector();

		Speed = FVector::DotProduct(forwordVec, velocity);

		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(rightVector, velocity);


		//auto movement = ownerPawn->GetMovement
		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}

void UPlayerAnimInstance::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}
