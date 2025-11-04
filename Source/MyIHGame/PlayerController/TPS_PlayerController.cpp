// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/TPS_PlayerController.h"
#include "../Player/TPS_PlayerState.h"
#include "../Instance/TPS_GameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "OnlineSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "../Data/IHDataSubsystem.h"
#include "../Player/TPS_PlayerState.h"
#include "../Instance/TPS_GameInstance.h"
#include "../Game/IHLobbyMode.h"
#include "../Character/IHPlayer.h"

void ATPS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}

	if (HasAuthority())
	{
		ATPS_PlayerState* characterState = GetPlayerState<ATPS_PlayerState>();
		UTPS_GameInstance* gameInstace = GetGameInstance<UTPS_GameInstance>();

		if (characterState == nullptr)
			return;


		characterState->SetPlayerName(gameInstace->PlayerName);

		APawn* controlledPawn = GetPawn();

		if (controlledPawn == nullptr)
			return;

		AIHPlayer* player = CastChecked<AIHPlayer>(controlledPawn);

		if (player != nullptr)
		{
			player->OnSetPlayerNameWidget(characterState->GetPlayerName());
		}
	}

}

void ATPS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocalController() && HasAuthority() == false)
	{
		LastTimeSynce += DeltaTime;

		if (LastTimeSynce > TimeSyncFrequence)
		{
			Server_RequestServerTime(GetWorld()->GetTimeSeconds());
			LastTimeSynce = 0;
		}
	}
}

void ATPS_PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ATPS_PlayerState* characterState = GetPlayerState<ATPS_PlayerState>();
	UTPS_GameInstance* gameInstace = GetGameInstance<UTPS_GameInstance>();

	if (characterState == nullptr)
		return;


	//characterState->SetPlayerName(gameInstace->PlayerName);

	APawn* controlledPawn = GetPawn();

	if (controlledPawn == nullptr)
		return;

	AIHPlayer* player = CastChecked<AIHPlayer>(controlledPawn);

	if (player != nullptr)
	{
		player->OnSetPlayerNameWidget(characterState->GetPlayerName());
	}

	Server_OnPlayerNameAssine(this, gameInstace->PlayerName);

}




void ATPS_PlayerController::Server_OnPlayerNameAssine_Implementation(ATPS_PlayerController* controller, const FString& name)
{
	ATPS_PlayerState* playerState = controller->GetPlayerState<ATPS_PlayerState>();
	if (playerState)
	{
		playerState->SetPlayerName(name);
		Client_BroadCastUserName(controller, name);
	}
	

}

void ATPS_PlayerController::Client_BroadCastUserName_Implementation(ATPS_PlayerController* controller, const FString& name)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPS_PlayerController::StaticClass(), OutActors);


	TArray<class ULocalPlayer*> test = GetGameInstance()->GetLocalPlayers();

	for (auto actor : OutActors)
	{
		if (ATPS_PlayerController* namedControler = CastChecked<ATPS_PlayerController>(actor))
		{
			if (namedControler != controller)
				continue;

			APawn* controlledPawn = namedControler->GetPawn();

			if (controlledPawn == nullptr)
				return;

			AIHPlayer* player = CastChecked<AIHPlayer>(controlledPawn);

			if (player != nullptr)
			{
				if (namedControler == controller)
				{
					player->OnSetPlayerNameWidget(name);

					ATPS_PlayerState* characterState = GetPlayerState<ATPS_PlayerState>();
				}

			}
		}
	}

}

void ATPS_PlayerController::Client_ShowGameResult_Implementation(EWinningTeam winningTeam)
{
	UTPS_GameInstance* gameInstace = GetGameInstance<UTPS_GameInstance>();
	if (gameInstace == nullptr)
		return;

	gameInstace->lastWinTeamType = winningTeam;
}

void ATPS_PlayerController::Client_MatchState_Implementation(float startTime, float matchTime)
{
	LevelStartTime = startTime;
	LevelMatchTime = matchTime;
}

//void ATPS_PlayerController::Client_MatchState(float startTime, float matchTime)
//{
//	LevelStartTime = startTime;
//	LevelMatchTime = matchTime;
//}

void ATPS_PlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	UTPS_GameInstance* GameInstacne = Cast< UTPS_GameInstance>(GetGameInstance());

	if (GameInstacne == nullptr)
		return;

	GameInstacne->SetPlayerReady(PlayerState->GetUniqueId());
	
}

float ATPS_PlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}

	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ATPS_PlayerController::ChangeCharacter()
{
	auto var = GetGameInstance();

	ATPS_PlayerState* PlayerSTate = GetPlayerState< ATPS_PlayerState>();

	if (!PlayerState)
		return;

	UIHDataSubsystem* DataSubsystem =  GetGameInstance()->GetSubsystem<UIHDataSubsystem>();
	if (!DataSubsystem)
		return;

	int32 NextIndex = (((int32)PlayerSTate->CharacterType) + 1) % DataSubsystem->CharacterAssetList.Num();
	Server_ChangeCharacter((ECharacterType)NextIndex);
}

void ATPS_PlayerController::Server_ChangeCharacter_Implementation(ECharacterType Charactertype)
{
	auto var = GetGameInstance();

	ATPS_PlayerState* PlayerSTate = GetPlayerState< ATPS_PlayerState>();
	if (!PlayerState)
		return;

	PlayerSTate->CharacterType = Charactertype;

	AIHLobbyGameeMode* LobbyMode = GetWorld()->GetAuthGameMode<AIHLobbyGameeMode>();

	LobbyMode->ChagnePlayerPawn(this, Charactertype);
}

void ATPS_PlayerController::Client_ReportServerTime_Implementation(float TimeOfClientRequest, float ServerTime)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = ServerTime + (RoundTripTime * 0.5f);

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ATPS_PlayerController::Server_RequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	Client_ReportServerTime(TimeOfClientRequest, ServerTime);
}

//void ATPS_PlayerController::Server_PlayerReady_Implementation()
//{
//	UTPS_GameInstance* GameInstacne = Cast< UTPS_GameInstance>(GetGameInstance());
//
//	if (GameInstacne == nullptr)
//		return;
//
//	GameInstacne->SetPlayerReady(PlayerState->GetUniqueId());
//}

void ATPS_PlayerController::Client_ReturnToMenu_Implementation()
{
	IOnlineSubsystem* subsystem = Online::GetSubsystem(GetWorld());
	{
		if (subsystem)
		{
			auto SessionInteface = subsystem->GetSessionInterface();
			SessionInteface->DestroySession(NAME_GameSession);
		}
	}
}

//void ATPS_PlayerController::ClientReturnToMainMenuWithTextReason(const FText& ReturnReason)
//{
//	/*IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
//	{
//		if (subsystem)
//		{
//			auto SessionInteface = subsystem->GetSessionInterface();
//			SessionInteface->DestroySession(NAME_GameSession);
//		}
//	}
//	Super::ClientReturnToMainMenuWithTextReason(ReturnReason);*/
//}



