// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "TPSGame/TPSPlayer.h"
#include "TPSGame/Enemy.h"
#include "TPSGame/EnemyAnim.h"
#include <Kismet/GameplayStatics.h>


// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...

	auto targetPlayer = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());

	Target = Cast<ATPSPlayer>(targetPlayer);

	Me = Cast<AEnemy>(GetOwner());

	EnemyAnim = Cast<UEnemyAnim>(Me->GetMesh()->GetAnimInstance());

}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsAlive)
		return;

	AttackTimer -= DeltaTime;

	FString logMsg = UEnum::GetValueAsString(State);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (State)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DeadState();
		break;
	case EEnemyState::State_Max:
		break;
	default:
		break;
	}

	// ...
}

void UEnemyFSM::IdleState()
{
	CurrentIdleTime += GetWorld()->GetDeltaSeconds();

	if (CurrentIdleTime >= MaxIdleTime)
	{
		State = EEnemyState::Move;
		CurrentIdleTime = 0.0f;

		EnemyAnim->EnemyState = State;
	}
}

void UEnemyFSM::MoveState()
{
	FVector targetLocation = Target->GetActorLocation();

	FVector moveDir = targetLocation - Me->GetActorLocation();

	FVector nomalizedVec = moveDir.GetSafeNormal();
	FVector forwardVec = GetOwner()->GetActorForwardVector();


	if (moveDir.Size() < AttackRange && AttackTimer <= 0.0f)
	{
		if (FVector::DotProduct(nomalizedVec, forwardVec) > 0.3)
		{
			State = EEnemyState::Attack;
		}
	}


	else
	{
		State = EEnemyState::Move;
		EnemyAnim->EnemyState = State;

		Me->AddMovementInput(moveDir);
		moveDir = FVector::Zero();
	}

}

void UEnemyFSM::AttackState()
{
	AttackTimer = AttackDuration;

	EnemyAnim->EnemyState = State;
	EnemyAnim->bAttackPlay = true;

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, "Attack!!");
	//State = EEnemyState::Move;

}

void UEnemyFSM::DamageState()
{
	CurrentDamageDelayTime -= GetWorld()->GetDeltaSeconds();
	EnemyAnim->EnemyState = EEnemyState::Damage;

	if (CurrentDamageDelayTime <= 0)
	{
		State = EEnemyState::Idle;
	}
}

void UEnemyFSM::DeadState()
{
	bIsAlive = false;

	EnemyAnim->PlayDamageAnim(FName("Die"));

}

void UEnemyFSM::onAttackAnimEnd()
{
	State = EEnemyState::Idle;
}

void UEnemyFSM::OnDamagedProcess()
{
	if (CurrentDamageDelayTime > 0)
	{
		return;
	}

	if (HP <= 0)
	{
		State = EEnemyState::Die;
		return;
	}

	HP--;
	State = EEnemyState::Damage;

	int32 index = FMath::RandRange(0, 1);
	FString SectionName = FString::Printf(TEXT("Damage%d"), index);
	EnemyAnim->PlayDamageAnim(FName(*SectionName));

	CurrentDamageDelayTime = DamageDelayTime;
}

void UEnemyFSM::OnDie()
{
	GetOwner()->Destroy();
}

