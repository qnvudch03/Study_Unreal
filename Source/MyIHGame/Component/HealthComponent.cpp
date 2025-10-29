// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/HealthComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraDataInterface.h"
#include "../Character/IHPlayer.h"
#include "../Animation/IHPlayerAnimInstance.h"
#include <Kismet/GameplayStatics.h>
#include "../Enemy/TPS_Minion.h"



// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	damageFloaterEffect2 = TSoftObjectPtr<UNiagaraSystem>(
		FSoftObjectPath(TEXT("/Game/Assets/Effects/Particles/Impacts/NS_DamageNumbers.NS_DamageNumbers")));

	SetIsReplicatedByDefault(true);

	DeathEffect = TSoftObjectPtr<UNiagaraSystem>(FSoftObjectPath(TEXT("/Game/Assets/Effects/Particles/Impacts/NS_DeathCubes.NS_DeathCubes")));


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
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
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
	AIHPlayer* ihPlayer = Cast<AIHPlayer>(character);
	if (ACharacter* HitCharacter = Cast<ACharacter>(GetOwner()))
		if (GetOwner())
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
		/*auto test0 = GetOwner()->GetLocalRole();
		auto test1 = GetOwner();
		auto test2 = OwningCharacter;
		auto test3 = OwningCharacter->GetController();*/
	}





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

}

void UHealthComponent::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (currentHP <= 0)
	{
		GetOwner()->OnTakePointDamage.RemoveDynamic(this, &UHealthComponent::TakePointDamage);

		/*FTimerHandle RespawnTimerHandle;


		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UHealthComponent::DeathTimerExpire, deathDelayTime, false);*/

		Multicast_death(DamageCauser);
	}

	currentHP = FMath::Clamp(currentHP - Damage, 0, maxHP);

	ShootInstigater = InstigatedBy;
	BeDamagedActor = DamagedActor;

	Multicast_ChangeCurrentHP(Damage, BeDamagedActor, ShootInstigater, HitLocation);
	
	APlayerController* MyPlayercontroller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (InstigatedBy == MyPlayercontroller) 
	{
		// 클라입장에서, 다른유저가 쏘면, IntigatedBy == nullptr; MyPlayerController = @
		// 서버입장에서, 다른유저가 쏘면, IntigatedBy == #; MyPlayerController = 
		// 내가 쐇다?
	}

	// 맞은건 어떻게?
	//MyPlayercontroller->GetPawn()



	APawn* damagedPawn = Cast<APawn>(DamagedActor);
	int currentControllers = UGameplayStatics::GetNumPlayerControllers(GetWorld());

	for (int i = 0; i < currentControllers; i++)
	{
		APlayerController* playercontroller = UGameplayStatics::GetPlayerController(GetWorld(), i);

		AController* controller = Cast<AController>(playercontroller);
		if (controller != nullptr && 
			((controller == InstigatedBy) || (controller == damagedPawn->GetController())))
		{
			AIHPlayer* ihPlayer = Cast<AIHPlayer>(controller->GetCharacter());
			if (ihPlayer)
			{
				ihPlayer->healthComp->Client_DamageFloat(Damage, HitLocation);
			}
		}

	}
}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (currentHP <= 0)
		return;

	currentHP = FMath::Clamp(currentHP - Damage, 0, maxHP);

	Multicast_ChangeCurrentHP(Damage, BeDamagedActor, ShootInstigater, DamagedActor->GetActorLocation());

	if (currentHP <= 0)
	{
		Multicast_death(DamageCauser);
	}
}

void UHealthComponent::PlayDamageEffect(float Damage, FVector HitLocation)
{
	/*UNiagaraComponent* NS_DamageFloat = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
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
	);*/
}

void UHealthComponent::Multicast_death_Implementation(AActor* DamgeCauser)
{
	OnDeathEventDelegate.Broadcast(DamgeCauser);

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsA(AIHPlayer::StaticClass()))
	{
		UIHPlayerAnimInstance* animInstance = Cast<UIHPlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());

		animInstance->PlayDeathAnim();
	}

	AIHPlayer* Player = Cast<AIHPlayer>(Character);
	if (Player)
	{
		Player->OnPlayDeathEffec();
	}

	/*if (Character->IsA(ATPS_Minion::StaticClass()))
	{
		Character->Destroy();
	}*/
}



void UHealthComponent::Client_DamageFloat_Implementation(float Damage, FVector HitLocation)
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

