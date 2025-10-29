// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/TPS_PlayerController.h"
#include "../Player/TPS_PlayerState.h"
#include "../Instance/TPS_GameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "OnlineSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "../Character/IHPlayer.h"

void ATPS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

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

void ATPS_PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

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

	Server_OnPlayerNameAssine(this, gameInstace->PlayerName);

}

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
				}
				
			}
		}
	}

}

void ATPS_PlayerController::Server_OnPlayerNameAssine_Implementation(ATPS_PlayerController* controller, const FString& name)
{
	auto var =  controller->GetGameInstance<UTPS_GameInstance>()->PlayerName;

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPS_PlayerController::StaticClass(), OutActors);

	for (auto var2 : OutActors)
	{
		FString Name = Cast<ATPS_PlayerController>(var2)->GetGameInstance<UTPS_GameInstance>()->PlayerName;

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, Name);
	}

	Client_BroadCastUserName(controller, name);

}
