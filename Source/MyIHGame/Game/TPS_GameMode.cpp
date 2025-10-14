// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_GameMode.h"
#include "../Player/TPS_PlayerState.h"
#include "../Character/TPS_Character.h"
#include "../PlayerController/TPS_PlayerController.h"
#include "../Enemy/TPS_Minion.h"
#include "../UI/IHHUD.h"
#include <Kismet/GameplayStatics.h>
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
