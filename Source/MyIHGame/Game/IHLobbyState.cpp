// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IHLobbyState.h"
#include "../Instance/TPS_GameInstance.h"
#include "../Game/TPS_GameMode.h"

void AIHLobbyState::MultiCast_ShowLoadingScreen_Implementation()
{
	UTPS_GameInstance* gameInstace = Cast< UTPS_GameInstance>(GetGameInstance());

	if (gameInstace != nullptr)
	{
		gameInstace->ShowLoadingScreen();
	}
}
