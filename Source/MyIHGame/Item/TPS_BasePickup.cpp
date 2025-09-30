// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPS_BasePickup.h"
#include "../Character/IHPlayer.h"
#include <Components/SphereComponent.h>

// Sets default values
ATPS_BasePickup::ATPS_BasePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = SphereCollision;
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(200.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ATPS_BasePickup::OnPickupBeginOverlap);

}

// Called when the game starts or when spawned
void ATPS_BasePickup::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void ATPS_BasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPS_BasePickup::OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const auto Character = Cast<AIHPlayer>(Other))
	{
		Pickup(Character);
	}
}

void ATPS_BasePickup::Pickup_Implementation(AIHPlayer* OwningCharacter)
{
	SetOwner(OwningCharacter);
}


