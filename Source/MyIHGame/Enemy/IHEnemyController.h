// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "IHEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API AIHEnemyController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	AIHEnemyController();

	TArray<AActor*> PatrolPoints;

	int32 CurrentPatrolIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAIPerceptionComponent> PawnSense;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, Category = "MY|Value")
	FName BBKey_bCanSeePlayer;

	UPROPERTY(EditAnywhere, Category = "MY|Value")
	FName BBKey_Player;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MY|Attack Anim Montage")
	class UAnimMontage* AttackAnimMontage;
	
};
