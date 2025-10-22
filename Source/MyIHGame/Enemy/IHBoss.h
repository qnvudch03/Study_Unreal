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

};
