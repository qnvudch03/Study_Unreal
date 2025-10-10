// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TPS_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API ATPS_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATPS_GameMode();

	UFUNCTION(BlueprintCallable, Category = "MY|Minions")
	void AlertMinions(class AActor* AlertInstigator, const FVector& Location, const float Radius);
	
};
