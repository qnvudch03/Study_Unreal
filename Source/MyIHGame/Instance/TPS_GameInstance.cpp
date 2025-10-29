// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/TPS_GameInstance.h"
#include "../PlayerController/TPS_PlayerController.h"
#include <Blueprint/UserWidget.h>
#include <MoviePlayer.h>
#include "../Data/IHDataSubsystem.h"
#include "../Game/IHLobbyState.h"
#include <GameFramework/GameMode.h>


static bool IsPIE(const UWorld* World)
{
	return GIsEditor && World && World->WorldType == EWorldType::PIE;
}

void UTPS_GameInstance::BeginLoadMap(const FWorldContext& worldContext, const FString& MapName)
{
	if (worldContext.OwningGameInstance != this)
		return;

	ShowLoadingScreen();
}

void UTPS_GameInstance::EndLoadMap(UWorld* InLoadeWorld)
{
	if (InLoadeWorld == nullptr || InLoadeWorld->GetGameInstance() != this)
		return;


	HideLoadingScreen();
}

void  UTPS_GameInstance::ShowLoadingScreen()
{
	UIHDataSubsystem* DataSubSystem = GetSubsystem<UIHDataSubsystem>();
	if (!DataSubSystem)
	{
		return;
	}

	if (IsPIE(GetWorld()))
	{
		if (LoadingWidget == nullptr)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(this, DataSubSystem->LoadingWidgetClass);
			LoadingWidget = Widget;
		}
		
		LoadingWidget->AddToViewport(1000);
	}

	else
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(this, DataSubSystem->LoadingWidgetClass);
		//Widget->AddToViewport(1000);

		FLoadingScreenAttributes Attr;
		Attr.bAutoCompleteWhenLoadingCompletes = true;
		Attr.MinimumLoadingScreenDisplayTime = 2.0f;
		Attr.WidgetLoadingScreen = Widget->TakeWidget();

		GetMoviePlayer()->SetupLoadingScreen(Attr);
	}
}

void  UTPS_GameInstance::HideLoadingScreen()
{
	if (IsPIE(GetWorld()))
	{
		TWeakObjectPtr<UTPS_GameInstance> temp = this;

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([temp]()
			{
				temp->LoadingWidget->RemoveFromParent();
			}
		), 2.0f, false);
	}

	else
	{
		GetMoviePlayer()->StopMovie();
	}
	
}

void UTPS_GameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UTPS_GameInstance::BeginLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTPS_GameInstance::EndLoadMap);
}

void UTPS_GameInstance::ExitGame()
{
	UWorld* world = GetWorld();

	if (!world)
		return;

	AGameMode* GameMode = world->GetAuthGameMode<AGameMode>();

	if (GameMode)
	{
		ATPS_PlayerController* Controller = NULL;
		FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
		for (; Iterator; ++Iterator)
		{
			Controller = Cast<ATPS_PlayerController>(Iterator->Get());
			if (Controller && !Controller->IsLocalPlayerController() && Controller->IsPrimaryPlayer())
			{
				// Clients
				Controller->Client_ReturnToMenu();
			}
		}

		GameMode->ReturnToMainMenuHost();
	}

	else
	{
		APlayerController* PlayerController = world->GetFirstPlayerController();

		if (PlayerController)
		{
			PlayerController->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Fress Exit")));
		}
	}
}

void UTPS_GameInstance::StartGame(bool TeamMatch, const FString& InURL)
{
	bTeamMatch = TeamMatch;

	UWorld* World = GetWorld();

	if (!World)
		return;

	AIHLobbyState* LobbyState = Cast< AIHLobbyState>(World->GetGameState());
	if (LobbyState)
	{
		LobbyState->MultiCast_ShowLoadingScreen();
	}

	World->ServerTravel(InURL);
}
