// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/PlayerBaseComponent.h"

// Sets default values for this component's properties
UPlayerBaseComponent::UPlayerBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
}


// Called when the game starts
void UPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	
	// ...
	
}


// Called every frame
void UPlayerBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Me = Cast<ATPSPlayer>(GetOwner());

	MoveComp = Cast< UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
	MoveComp = Me->GetCharacterMovement();

	Me->SetCompControllDelegate.AddUObject(this, &UPlayerBaseComponent::SetupInputBinding);
}

