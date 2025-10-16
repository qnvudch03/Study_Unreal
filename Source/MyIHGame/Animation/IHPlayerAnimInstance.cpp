// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/IHPlayerAnimInstance.h"
#include "Character/IHPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/IHPlayerFireComponent.h"
#include "Weapon/IHWeapon.h"
#include "Util/MyIHGame.h"

void UIHPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwnerPawn = TryGetPawnOwner();
	AIHPlayer* Player = Cast<AIHPlayer>(OwnerPawn);
	if (Player)
	{

		Velocity = Player->GetVelocity();
		FVector ForwardVector = Player->GetActorForwardVector();

		// Velocity 자체의 스피드
		Speed = Velocity.Length();

		//Speed = Player->GetCharacterMovement()->MaxWalkSpeed;

		if (Player->HasAuthority() && Player->GetRemoteRole() == ENetRole::ROLE_AutonomousProxy)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Authority Speed : %f"), Speed));
			//PRINT_LOG(TEXT("Authority Speed : %f"),Speed);
		}

		else if (!Player->HasAuthority() && Player->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Client Speed : %f"), Speed));
			//PRINT_LOG(TEXT("Client Speed : %f"), Speed);
		}

		// 가고자 하는 방향 Velocity는 월드 기준.
		// 월드 기준이 아니라, 플레이어 로컬공간의 회전값을 얻기위해 역행렬을 곱해준다.
		Direction = Player->GetActorTransform().InverseTransformVector(Velocity).Rotation().Yaw;


		UCharacterMovementComponent* Movement = Player->GetCharacterMovement();
		bIsInAir = Movement->IsFalling();
		bIsAccelerating = Movement->GetCurrentAcceleration().Size() > 0.f ? true : false;

		// Player 컴포넌트에서 계산된 AO 값 애니메이션에 전달
		AO_Yaw = Player->FireComp->AimOffset_Yaw;
		AO_Pitch = Player->FireComp->AimOffset_Pitch;

		// 왼손이 있어야하는 위치를 계산한다.
		// Weapon 값이 nullptr 체크 + 변수로 활용
		if (AIHWeapon* Weapon = Player->FireComp->Weapon) // 무기가 있는지 확인
		{
			// 총기의 왼손 위치 소켓을 가져온다.
			LeftHandTransfrom = Weapon->WeaponMesh->GetSocketTransform(TEXT("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

			// 캐릭터 기준으로 변환된 위치와 회전값
			FVector Position;
			FRotator Rotator;

			Player->GetMesh()->TransformToBoneSpace(
				FName("hand_r"),
				LeftHandTransfrom.GetLocation(),
				FRotator::ZeroRotator,
				Position,
				Rotator);

			// hand_r 공간으로 변환된 로테이션
			LeftHandTransfrom.SetLocation(Position);
			LeftHandTransfrom.SetRotation(FQuat(Rotator));

			TurnInPlaceType = Player->FireComp->TurnInPlaceType;
		}
	}
}

void UIHPlayerAnimInstance::PlayAttackAnim(bool bIsRun)
{
	// TEST
	FName SectionName = bIsRun ? TEXT("Hip") : TEXT("Ironsights");
	Montage_Play(AttackAnimMontage);
	Montage_JumpToSection(SectionName, AttackAnimMontage);
}

void UIHPlayerAnimInstance::PlayDeathAnim()
{
	int randInt = FMath::RandRange(0, 1);

	FName SectionName = (randInt == 0) ? TEXT("Death_A") : TEXT("Death_B");

	Montage_Play(DeathAnimMontage);
	Montage_JumpToSection(SectionName, AttackAnimMontage);
}

void UIHPlayerAnimInstance::PlayDrawingAnim()
{
	Montage_Play(DrawWeaponAnimMontage);
}
