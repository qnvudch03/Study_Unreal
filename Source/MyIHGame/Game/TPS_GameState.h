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

	UPROPERTY(Replicated)
	int32 BlueKill = 0;

	UPROPERTY(Replicated)
	int32 RedKill = 0;
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
