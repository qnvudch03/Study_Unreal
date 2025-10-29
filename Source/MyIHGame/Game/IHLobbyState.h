// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "IHLobbyState.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API AIHLobbyState : public AGameState
{
	GENERATED_BODY()

public:
	/*UFUNCTION(NetMulticast, Reliable)
	void MultiCast_ShowLoadingScree();*/

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_ShowLoadingScreen();

};
