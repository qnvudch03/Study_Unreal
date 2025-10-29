// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IHLobbyMode.h"
#include "../Data/IHDataSubsystem.h"
#include "../Character/IHPlayer.h"
#include "../Player/TPS_PlayerState.h"

void AIHLobbyMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (NewPlayer)
	{
		UIHDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UIHDataSubsystem>();

		if (DataSubsystem == nullptr)
		{
			return;
		}

		AIHPlayer* Player = Cast< AIHPlayer>(NewPlayer->GetPawn());
		ATPS_PlayerState* playerState = Cast< ATPS_PlayerState>(Player->GetPlayerState());

		if (playerState->SkinIndex == -1)
		{
			playerState->SkinIndex = SkinCount;
			Player->ApplyCharacterSkin();
		}

		SkinCount = (SkinCount + 1) % DataSubsystem->SkinAssetList.Num();
	}
}