// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_GameState.h"
#include <Net/UnrealNetwork.h>

void ATPS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPS_GameState, BlueKill);
	DOREPLIFETIME(ATPS_GameState, RedKill);
}
