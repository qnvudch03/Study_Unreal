// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHBoss.h"
#include "IHEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Component/HealthComponent.h"
#include "../Character/IHPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <BrainComponent.h>
#include <Components/SphereComponent.h>

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

	leftHandCollider = CreateDefaultSubobject<USphereComponent>(TEXT("hand_L_Collider"));
	leftHandCollider->SetupAttachment(GetMesh(), TEXT("hand_l"));
	leftHandCollider->SetSphereRadius(32.0f);
	leftHandCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	leftHandCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	leftHandCollider->SetActive(false);

	rightHandCollider = CreateDefaultSubobject<USphereComponent>(TEXT("hand_R_Collider"));
	rightHandCollider->SetupAttachment(GetMesh(), TEXT("hand_r"));
	rightHandCollider->SetSphereRadius(32.0f);
	rightHandCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	rightHandCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	rightHandCollider->SetActive(false);
}

// Called when the game starts or when spawned
void AIHBoss::BeginPlay()
{
	Super::BeginPlay();

	leftHandCollider->OnComponentBeginOverlap.AddDynamic(this, &AIHBoss::OnOverlapLeftHandCollision);
	rightHandCollider->OnComponentBeginOverlap.AddDynamic(this, &AIHBoss::OnOverlapRightHandCollision);

	healthComp->OnDeathEventDelegate.AddUObject(this, &AIHBoss::onDeathEvent);

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

void AIHBoss::OnOverlapLeftHandCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AIHPlayer* player = Cast<AIHPlayer>(OtherActor);

	if (player == nullptr)
		return;

	UGameplayStatics::ApplyDamage(player, damage, GetController(), this, UDamageType::StaticClass());
}

void AIHBoss::OnOverlapRightHandCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AIHPlayer* player = Cast<AIHPlayer>(OtherActor);

	if (player == nullptr)
		return;

	UGameplayStatics::ApplyDamage(player, damage, GetController(), this, UDamageType::StaticClass());
}

void AIHBoss::SetHandColliderActive()
{
	leftHandCollider->SetActive(true);
	rightHandCollider->SetActive(true);
}

void AIHBoss::SetHandColliderDeActive()
{
	leftHandCollider->SetActive(false);
	rightHandCollider->SetActive(false);
}

void AIHBoss::onDeathEvent(AActor* DameCauser)
{
	if (HasAuthority())
	{
		if (healthComp)
		{
			SetLifeSpan(healthComp->deathDelayTime);
		}

		else
		{
			Destroy();
		}
	}

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
		if (UBrainComponent* Brain = AI->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Death"));
		}
	}

	UAnimInstance* AnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance == nullptr)
		return;

	AnimInstance->Montage_Play(deathMontage);

	OnPlayDeathEffec();


	//·¢µ¶
	TArray<UActorComponent*> Components;
	GetComponents(UPrimitiveComponent::StaticClass(), Components);

	for (UActorComponent* Comp : Components)
	{
		UPrimitiveComponent* Primitive = Cast< UPrimitiveComponent>(Comp);
		if (Primitive)
		{
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}



