// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionTypes.h"
#include "TPS_Minion.generated.h"

UCLASS()
class MYIHGAME_API ATPS_Minion : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPS_Minion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	UFUNCTION(BlueprintCallable, Category = "MY|Minion AI")
	void GoToLocation(const FVector& Location);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Minion AI")
	float PatrolSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Minion AI")
	float ChaseSpeed = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Minion AI")
	float PatrolRadius = 50000.0f;

	UFUNCTION(BlueprintCallable, Category = "MY|Minion AI")
	void SetNextPosition();

	UFUNCTION(BlueprintCallable, Category = "MY|Minion AI")
	void Chase(APawn* Player);

	virtual void PostInitializeComponents() override;

	FORCEINLINE class UAIPerceptionComponent* GetPawnSense() { return PawnSense; }

	FORCEINLINE class USphereComponent* GetCollider() { return Collision; }

	UFUNCTION()
	void OnCharacterDetected(APawn* Player);

	UFUNCTION()
	void OnCatchPlayer(AActor* OverlappedActor, AActor* OtherActor);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAIPerceptionComponent> PawnSense;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY()
	FVector PatrolLocation;

	UPROPERTY();
	FTimerHandle MinionAttackTimerHandle;

	float attackInterval = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Minion AI", meta = (AllowPrivateAccess = "true"))
	float AlertRadius = 6000.0f;

public:
	//Combat
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Combat")
	float Health = 5.0f;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MY|HealthComponent")
	class UHealthComponent* healthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Combat")
	TSubclassOf<class ATPS_BasePickup> SpawnedPickup;

	UFUNCTION()
	void OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void OnDeathEvent(AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Health)
	void OnMionDeathEffec();

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
