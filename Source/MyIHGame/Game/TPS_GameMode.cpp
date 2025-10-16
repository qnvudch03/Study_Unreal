// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_GameMode.h"
#include "../Player/TPS_PlayerState.h"
#include "../Character/TPS_Character.h"
#include "../PlayerController/TPS_PlayerController.h"
#include "../Enemy/TPS_Minion.h"
#include "../UI/IHHUD.h"
#include <GameFramework/PlayerStart.h>
#include <Kismet/GameplayStatics.h>
#include "../Data/IHDataSubsystem.h"
#include "../Player/TPS_PlayerState.h"
#include "../Component/IHPlayerFireComponent.h"
#include "../Weapon/IHWeapon.h"
#include "../Character/IHPlayer.h"
ATPS_GameMode::ATPS_GameMode()
{
	GameStateClass = ATPS_GameMode::StaticClass();

	PlayerStateClass = ATPS_PlayerState::StaticClass();
	PlayerControllerClass = ATPS_PlayerController::StaticClass();

	HUDClass = AIHHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));

	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATPS_GameMode::AlertMinions(AActor* AlertInstigator, const FVector& Location, const float Radius)
{
	TArray<AActor*> Minions;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPS_Minion::StaticClass(), Minions);

	if (Minions.Num() > 0)
	{
		for (const auto Minion : Minions)
		{
			if (Minion == AlertInstigator)
				continue;

			auto Distance = FVector::Distance(AlertInstigator->GetActorLocation(), Minion->GetActorLocation());

			if (Distance < Radius)
			{
				static_cast<ATPS_Minion*>(Minion)->GoToLocation(Location);
			}
		}
	}
}

void ATPS_GameMode::RestartPlayer(AController* NewPlayer)
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

void ATPS_GameMode::PlayerRespawn(ACharacter* DeathCharacter)
{
	if (DeathCharacter == nullptr)
		return;

	AController* Controller = DeathCharacter->GetController();
	int32 RandIndex = 0;
	TArray<AActor*> PlayerStatActors;

	if (Controller)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStatActors);

		RandIndex = FMath::RandRange(0, PlayerStatActors.Num() - 1);
	}

	if (DeathCharacter)
	{
		AIHPlayer* deadCharacter = Cast<AIHPlayer>(DeathCharacter);
		deadCharacter->FireComp->Weapon->Destroy();

		//DeathCharacter->Reset();
		DeathCharacter->Destroy();

	}

	RestartPlayerAtPlayerStart(Controller, PlayerStatActors[RandIndex]);
}
