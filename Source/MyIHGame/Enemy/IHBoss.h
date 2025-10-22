// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IHBoss.generated.h"

UCLASS()
class MYIHGAME_API AIHBoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIHBoss();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MY|HealthComponent")
	class UHealthComponent* healthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MY|Value")
	float PatrolSpeed = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MY|Value")
	float PatrolRadius = 1500;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MY|Value")
	float ChaseSpeed = 500;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable)
	void UpdateWalkSpeed(float Speed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MY|BossAtkMontage")
	TObjectPtr<class UAnimMontage> attackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MY|BossDeathMontage")
	TObjectPtr<class UAnimMontage> deathMontage;

public:
	//Attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MY|LeftHand")
	TObjectPtr<class USphereComponent> leftHandCollider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MY|LeftHand")
	TObjectPtr<class USphereComponent> rightHandCollider;

	UFUNCTION()
	void OnOverlapLeftHandCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapRightHandCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetHandColliderActive();

	UFUNCTION(BlueprintCallable)
	void SetHandColliderDeActive();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MY|Value")
	float damage = 15;

	void onDeathEvent(AActor* DameCauser);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Health)
	void OnPlayDeathEffec();

};
