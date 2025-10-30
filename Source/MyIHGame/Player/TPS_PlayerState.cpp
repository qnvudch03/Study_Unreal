// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TPS_PlayerState.h"
#include "../CharacterStat/CharacterStat.h"
#include "../Character/IHPlayer.h"
#include "../PlayerController/TPS_PlayerController.h"
#include <Net/UnrealNetwork.h>

void ATPS_PlayerState::AddXp(int32 value)
{
	Xp += value;
	OnXpChanged.Broadcast(Xp);

	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Yellow, FString::Printf(TEXT("Totla Xp : %d"), Xp));
	
	if (const auto Character = Cast<AIHPlayer>(GetPawn()))
	{
		if (Character->GetCharacterStat()->NextLevelXp > Xp)
		{
			Lv++;
			Character->UpdateCharacterStat(Lv);
			OnLvChagned.Broadcast(Lv);

			GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, FString::Printf(TEXT("Level UP!")));
		}
	}
}

void ATPS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ATPS_PlayerState, Xp);
	DOREPLIFETIME_CONDITION(ATPS_PlayerState, Xp, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATPS_PlayerState, Lv, COND_OwnerOnly);

	DOREPLIFETIME(ATPS_PlayerState, SkinIndex);
	DOREPLIFETIME(ATPS_PlayerState, Team);

}

void ATPS_PlayerState::OnRep_Xp()
{
	OnXpChanged.Broadcast(Xp);
}

void ATPS_PlayerState::OnRep_Lv()
{
	OnLvChagned.Broadcast(Lv);
}

void ATPS_PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ATPS_PlayerState* NewPlayerState = Cast<ATPS_PlayerState>(PlayerState);

	if (NewPlayerState)
	{
		NewPlayerState->SkinIndex = SkinIndex;
	}
}

void ATPS_PlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	//player->OnSetPlayerNameWidget(characterState->GetPlayerName());

	if (true)
	{
		APlayerController* controller = GetPlayerController();
		if (controller == nullptr)
			return;

		APawn* controlledPawn = Cast<ATPS_PlayerController>(controller)->GetPawn();

		if (controlledPawn == nullptr)
			return;

		AIHPlayer* player = CastChecked<AIHPlayer>(controlledPawn);

		if (player != nullptr)
		{
			player->OnSetPlayerNameWidget(GetPlayerName());
		}

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, GetPlayerName());


	}

	
}
