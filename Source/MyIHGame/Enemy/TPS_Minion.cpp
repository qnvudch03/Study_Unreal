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
#include "../Game/TPS_GameMode.h"
#include "../Item/TPS_BasePickup.h"

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

	static ConstructorHelpers::FClassFinder<ATPS_BasePickup> SpawnedPickupAsset(TEXT("/Game/Blueprints/Item/BP_GoldCoin"));
	if (SpawnedPickupAsset.Class)
	{
		SpawnedPickup = SpawnedPickupAsset.Class;
	}

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

void ATPS_Minion::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Noise Detected!"));
	GoToLocation(Location);
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

	if (const auto GameMode = Cast<ATPS_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->AlertMinions(this, Player->GetActorLocation(), AlertRadius);
	}

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Player);

	DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 25.0f, 12, FColor::Red, true, 10.0f, 0, 2.0f);
}

void ATPS_Minion::GoToLocation(const FVector& Location)
{
	PatrolLocation = Location;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
}

void ATPS_Minion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	OnActorBeginOverlap.AddDynamic(this, &ATPS_Minion::OnCatchPlayer);
	GetPawnSense()->OnSeePawn.AddDynamic(this, &ATPS_Minion::OnCharacterDetected);
	GetPawnSense()->OnHearNoise.AddDynamic(this, &ATPS_Minion::OnHearNoise);
	OnTakeAnyDamage.AddDynamic(this, &ATPS_Minion::OnDamage);
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

void ATPS_Minion::OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health -= Damage;

	if (Health > 0)
		return;

	if (SpawnedPickup)
	{
		GetWorld()->SpawnActor<ATPS_BasePickup>(SpawnedPickup, GetActorLocation(), GetActorRotation());
	}

	Destroy();
}

