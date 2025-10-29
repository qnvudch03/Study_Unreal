// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/IHLobbyGameeMode.h"
#include "IHLobbyMode.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API AIHLobbyMode : public AIHLobbyGameeMode
{
	GENERATED_BODY()

private:
	int32 SkinCount = 0;
	
public:
	void RestartPlayer(AController* NewPlyaer) override;

	void PlayerRespawn(ACharacter* DeathCharacter);
};
