// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TPS_GameState.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API ATPS_GameState : public AGameState
{
	GENERATED_BODY()

public:
	TArray<class ATPS_PlayerState*> RedTeam;
	TArray<class ATPS_PlayerState*> BlueTeam;
	
};
