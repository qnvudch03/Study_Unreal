// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TPS_Minion.h"
#include "../Character/IHPlayer.h"

#include <AIController.h>
#include <NavigationSystem.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Perception/PawnSensingComponent.h>
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <Components/SphereComponent.h>

// Sets default values
ATPS_Minion::ATPS_Minion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	PawnSense = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSense"));
	PawnSense->SensingInterval = 0.8f;
	PawnSense->SetPeripheralVisionAngle(45.0f);
	PawnSense->SightRadius = 1500.0f;
	PawnSense->HearingThreshold = 400.0f;
	PawnSense->LOSHearingThreshold = 800.0f;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(100);
	Collision->SetupAttachment(RootComponent);

	GetCapsuleComponent()->InitCapsuleSize(60.0f, 96.0f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -91.0f));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletonWarriorMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/KayKit/Skeletons/skeleton_warrior.skeleton_warrior'"));

	if (skeletonWarriorMesh.Object)
	{
		GetMesh()->SetSkeletalMesh(skeletonWarriorMesh.Object);
		GetMesh()->SetupAttachment(Collision);
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

}

// Called when the game starts or when spawned
void ATPS_Minion::BeginPlay()
{
	Super::BeginPlay();

	SetNextPosition();
	
}

// Called every frame
void ATPS_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	if (GetMovementComponent()->GetMaxSpeed() == ChaseSpeed)
		return;

	if ((GetActorLocation() - PatrolLocation).Size() < 500.0f)
	{
		SetNextPosition();
	}
}

// Called to bind functionality to input
void ATPS_Minion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATPS_Minion::SetNextPosition()
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	const auto LocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, GetActorLocation(), PatrolLocation, PatrolRadius);

	if (LocationFound)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
	}
}

void ATPS_Minion::Chase(APawn* Player)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Player);

	DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 25.0f, 12, FColor::Red, true, 10.0f, 0, 2.0f);
}

void ATPS_Minion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	OnActorBeginOverlap.AddDynamic(this, &ATPS_Minion::OnCatchPlayer);
	GetPawnSense()->OnSeePawn.AddDynamic(this, &ATPS_Minion::OnCharacterDetected);
}

void ATPS_Minion::OnCharacterDetected(APawn* Player)
{
	if (!Player->IsA<AIHPlayer>())
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Character Detected!"));

	if (GetCharacterMovement()->MaxWalkSpeed != ChaseSpeed)
	{
		Chase(Player);
	}
}

void ATPS_Minion::OnCatchPlayer(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsA<AIHPlayer>())
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Character captured!"));

	static_cast<AIHPlayer*>(OtherActor)->OnHitEvent();

	GetWorld()->GetTimerManager().SetTimer(MinionAttackTimerHandle, this, &ATPS_Minion::SetNextPosition, attackInterval);

}

