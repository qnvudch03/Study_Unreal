// Fill out your copyright notice in the Description page of Project Settings.

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraDataInterface.h"
#include "../Character/IHPlayer.h"
#include "../Animation/IHPlayerAnimInstance.h"

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
	DOREPLIFETIME(UHealthComponent, BeDamagedActor);
}

#include <Engine/StreamableManager.h>
#include <Engine/AssetManager.h>
void UHealthComponent::Multicast_ChangeCurrentHP_Implementation(float Damage, AActor* hittedactor, AController* InstigatedBy, FVector_NetQuantize HitLocation)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (ACharacter* HitCharacter = Cast<ACharacter>(GetOwner()))
	if(GetOwner())
	{
		for (int32 i = 0; i < HitCharacter->GetMesh()->GetMaterials().Num(); i++)
		{
			UMaterialInstanceDynamic* DynamicMaterial = HitCharacter->GetMesh()->CreateDynamicMaterialInstance(i);

			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(FName("HitFxSwitch"), 1.0f);
				DynamicMaterial->SetVectorParameterValue(FName("HitEmissiveColor"), FVector(0, 0, 1));
			}
		}
		GetWorld()->GetTimerManager().ClearTimer(HitFXTimerHander);
		GetWorld()->GetTimerManager().SetTimer(HitFXTimerHander, this, &UHealthComponent::ResetHitFxTimer, 0.5);
	}

	{
		auto test0 = GetOwner()->GetLocalRole();
		auto test1 = GetOwner();
		auto test2 = OwningCharacter;
		auto test3 = OwningCharacter->GetController();
	}




	if (GetOwner() == hittedactor) //(GetOwner() == hittedactor))
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
	{
		GetOwner()->OnTakePointDamage.RemoveDynamic(this, &UHealthComponent::TakePointDamage);

		FTimerHandle RespawnTimerHandle;

		Multicast_death();
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UHealthComponent::DeathTimerExpire, deathDelayTime, false);
	}

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

void UHealthComponent::Multicast_death_Implementation()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		UIHPlayerAnimInstance* animInstance = Cast<UIHPlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());

		animInstance->PlayDeathAnim();
	}

	AIHPlayer* Player = Cast<AIHPlayer>(Character);
	if (Player)
	{
		Player->OnPlayDeathEffec();
	}
}

void UHealthComponent::DeathTimerExpire()
{
	ATPS_GameMode* GameMode = Cast<ATPS_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->PlayerRespawn(Cast<ACharacter>(GetOwner()));
	}

	return;
}

void UHealthComponent::ResetHitFxTimer()
{
	if (ACharacter* HitCharacter = Cast<ACharacter>(GetOwner()))
	{
		for (int32 i = 0; i < HitCharacter->GetMesh()->GetMaterials().Num(); i++)
		{
			UMaterialInstanceDynamic* DynamicMaterial = HitCharacter->GetMesh()->CreateDynamicMaterialInstance(i);

			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(FName("HitFxSwitch"), 0.f);
			}
		}
	}
}

