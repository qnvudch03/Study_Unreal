// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float PatrolSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float ChaseSpeed = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float PatrolRadius = 50000.0f;

	UFUNCTION(BlueprintCallable, Category = "Minion AI")
	void SetNextPosition();

	UFUNCTION(BlueprintCallable, Category = "Minion AI")
	void Chase(APawn* Player);

	virtual void PostInitializeComponents() override;

	FORCEINLINE class UPawnSensingComponent* GetPawnSense() { return PawnSense; }

	FORCEINLINE class USphereComponent* GetCollider() { return Collision; }

	UFUNCTION()
	void OnCharacterDetected(APawn* Player);

	UFUNCTION()
	void OnCatchPlayer(AActor* OverlappedActor, AActor* OtherActor);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPawnSensingComponent> PawnSense;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY()
	FVector PatrolLocation;

	UPROPERTY();
	FTimerHandle MinionAttackTimerHandle;

	float attackInterval = 0.8f;

};
