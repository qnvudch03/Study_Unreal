// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IHLobbyGameeMode.h"
#include "../Character/IHPlayer.h"
#include "../Data/IHDataSubsystem.h"
#include "../Player/TPS_PlayerState.h"
#include "IHLobbyState.h"

AIHLobbyGameeMode::AIHLobbyGameeMode()
{
	DefaultPawnClass = AIHPlayer::StaticClass();

	GameStateClass = AIHLobbyState::StaticClass();

	bUseSeamlessTravel = true;
}

void AIHLobbyGameeMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	/*if (NewPlayer)
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
	}*/
}

void AIHLobbyGameeMode::ChagnePlayerPawn(AController * Player, ECharacterType characterType)
{
	UGameInstance* gameInstance = GetGameInstance();

	UIHDataSubsystem* DataSubSystme = GetGameInstance()->GetSubsystem<UIHDataSubsystem>();

	if (!DataSubSystme)
		return;

	if (!Player)
		return;


	/*auto Find = DataSubSystme->CharacterAssetList.FindByPredicate([characterType](const UIHDataAsset& Info)
		{
			return Info.Type == characterType;
		}
	);*/

	UIHDataAsset* FindAsset = nullptr;
	for (auto Iter : DataSubSystme->CharacterAssetList)
	{
		if (Iter->Type == characterType)
		{
			FindAsset = Iter;
			break;
		}
	}


	APawn* OldPawn = Player->GetPawn();
	if (!OldPawn)
		return;

	FVector SpawnLocation = OldPawn->GetActorLocation();
	FRotator SpawnRotator = OldPawn->GetActorRotation();

	OldPawn->UnPossessed();
	OldPawn->Destroy();

	FActorSpawnParameters SpawnRarams;
	SpawnRarams.Owner = Player;


	AIHPlayer* Character = GetWorld()->SpawnActor<AIHPlayer>((FindAsset)->BlueprintAsset, SpawnLocation, SpawnRotator, SpawnRarams);
	Character->PossessedBy(Player);
}