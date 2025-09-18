// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/Enemy.h"

#include <EnemyFSM.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "EnemyAnim.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> EnemySkeleton(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/RPG_Mercenary_Character/Mesh/SKM_Mercenary.SKM_Mercenary'"));

	if (EnemySkeleton.Object)
	{
		GetMesh()->SetSkeletalMesh(EnemySkeleton.Object);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 00));
		GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	}

	{
		ConstructorHelpers::FClassFinder<UEnemyAnim> EnemyAnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/TPSGame/Animation/Enemy/ABP_EnemyAnim.ABP_EnemyAnim_C'"));

		if (EnemyAnimBP.Class)
		{
			GetMesh()->SetAnimInstanceClass(EnemyAnimBP.Class);
		}
	}

	{
		FsmComp = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));
	}

	{
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

