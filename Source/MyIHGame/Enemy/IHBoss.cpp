// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHBoss.h"
#include "IHEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Component/HealthComponent.h"

// Sets default values
AIHBoss::AIHBoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	healthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	bReplicates = true;

	AIControllerClass = AIHEnemyController::StaticClass();
	//bossAIController;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AIHBoss::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIHBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AIHBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AIHBoss::UpdateWalkSpeed(float Speed)
{
	/*UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (MovementComp)
	{
		return MovementComp->GetMaxWalk
	}*/

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

