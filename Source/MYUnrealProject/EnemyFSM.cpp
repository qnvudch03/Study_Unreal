// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "TPSGame/TPSPlayer.h"
#include "TPSGame/Enemy.h"
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
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
	}
}

void UEnemyFSM::MoveState()
{
	FVector targetLocation = Target->GetActorLocation();

	FVector moveDir = targetLocation - Me->GetActorLocation();

	if (moveDir.Size() < AttackRange && AttackTimer <= 0.0f)
	{
		State = EEnemyState::Attack;

	}

	else
	{
		Me->AddMovementInput(moveDir);
		moveDir = FVector::Zero();
	}
	
}

void UEnemyFSM::AttackState()
{
	AttackTimer = AttackDuration;

	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, "Attack!!");
	State = EEnemyState::Move;
	UE_LOG(LogTemp, Log, TEXT("Attack!"));

}

void UEnemyFSM::DamageState()
{
	CurrentDamageDelayTime -= GetWorld()->GetDeltaSeconds();

	if (CurrentDamageDelayTime <= 0)
	{
		State = EEnemyState::Idle;
	}
}

void UEnemyFSM::DeadState()
{
	Me->Destroy();
}

void UEnemyFSM::OnDamagedProcess()
{
	if (CurrentDamageDelayTime > 0)
	{
		return;
	}

	HP--;
	State = EEnemyState::Damage;

	if (HP <= 0)
	{
		State = EEnemyState::Die;
	}
	
	CurrentDamageDelayTime = DamageDelayTime;
}

