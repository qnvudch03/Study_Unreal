// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_GameMode.h"
#include "../Player/TPS_PlayerState.h"
#include "../Character/TPS_Character.h"
#include "../PlayerController/TPS_PlayerController.h"

ATPS_GameMode::ATPS_GameMode()
{
	GameStateClass = ATPS_GameMode::StaticClass();

	PlayerStateClass = ATPS_PlayerState::StaticClass();
	PlayerControllerClass = ATPS_PlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));

	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
