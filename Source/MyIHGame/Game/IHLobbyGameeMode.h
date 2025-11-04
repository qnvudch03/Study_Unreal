// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "../Util/IHDefine.h"
#include "IHLobbyGameeMode.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API AIHLobbyGameeMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	AIHLobbyGameeMode();

private:
	int32 SkinCount = 0;

public:
	void RestartPlayer(AController* NewPlyaer) override;

	void PlayerRespawn(ACharacter* DeathCharacter);

	void ChagnePlayerPawn(AController* Player, ECharacterType characterType);
	
};
