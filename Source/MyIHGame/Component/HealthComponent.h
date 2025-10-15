// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Net/UnrealNetwork.h>
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYIHGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Value")
	int32 currentHP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Value")
	int32 maxHP = 100;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeCurrentHP(float Damage, AActor* hittedactor, class AController* InstigatedBy, FVector_NetQuantize HitLocation);

	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UPROPERTY(Replicated)
	class AController* ShootInstigater;

	UPROPERTY(Replicated)
	class AActor* BeDamagedActor;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Damage Floater")
	class UNiagaraSystem* damageFloaterEffect;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Damage Floater")
	TSoftObjectPtr<class UNiagaraSystem> damageFloaterEffect2;

	void PlayDamageEffect(float Damage ,FVector HitLocation);

	UPROPERTY()
	TObjectPtr<class AIHPlayer> OwningCharacter;

};
