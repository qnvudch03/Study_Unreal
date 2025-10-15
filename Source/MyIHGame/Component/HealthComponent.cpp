// Fill out your copyright notice in the Description page of Project Settings.

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraDataInterface.h"
#include "../Character/IHPlayer.h"

#include "Component/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	damageFloaterEffect2 = TSoftObjectPtr<UNiagaraSystem>(
		FSoftObjectPath(TEXT("/Game/Assets/Effects/Particles/Impacts/NS_DamageNumbers.NS_DamageNumbers")));

	SetIsReplicatedByDefault(true);


	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	currentHP = maxHP;

	if (GetOwner()->HasAuthority())
	{
		GetOwner()->OnTakePointDamage.AddDynamic(this, &UHealthComponent::TakePointDamage);
	}

	OwningCharacter = Cast<AIHPlayer>(GetOwner());

}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, currentHP);
}

#include <Engine/StreamableManager.h>
#include <Engine/AssetManager.h>
void UHealthComponent::Multicast_ChangeCurrentHP_Implementation(float Damage, AActor* hittedactor, AController* InstigatedBy, FVector_NetQuantize HitLocation)
{
	auto test0 = GetOwner()->GetLocalRole();
	auto test1 = GetOwner();
	auto test2 = OwningCharacter;
	auto test3 = OwningCharacter->GetController();

	if (true) //(GetOwner() == hittedactor))
	{
		if (damageFloaterEffect2.IsValid())
		{
			TWeakObjectPtr<UHealthComponent> WeakThis = this;

			FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
			StreamableManager.RequestAsyncLoad(damageFloaterEffect2.ToSoftObjectPath(),
				FStreamableDelegate::CreateLambda(
					[WeakThis, Damage, HitLocation]()
					{
						if (WeakThis.IsValid())
						{
							WeakThis->PlayDamageEffect(Damage, HitLocation);
						}
					}
				));
		}

		else
		{

		}
	}


	


	

	//currentHP = FMath::Clamp(currentHP - Damage, 0, maxHP);
}

void UHealthComponent::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (currentHP <= 0)
		return;

	currentHP = FMath::Clamp(currentHP - Damage, 0, maxHP);

	ShootInstigater = InstigatedBy;
	BeDamagedActor = DamagedActor;

	Multicast_ChangeCurrentHP(Damage, BeDamagedActor, ShootInstigater, HitLocation);
}

void UHealthComponent::PlayDamageEffect(float Damage, FVector HitLocation)
{
	UNiagaraComponent* NS_DamageFloat = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		damageFloaterEffect2.Get(),
		HitLocation
	);

	TArray<FVector4> VectorArray;
	FVector4 ParamValue(HitLocation.X, HitLocation.Y, HitLocation.Z, Damage);
	VectorArray.Add(ParamValue);

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(
		NS_DamageFloat,
		FName("DamageInfo"),
		VectorArray
	);
}

