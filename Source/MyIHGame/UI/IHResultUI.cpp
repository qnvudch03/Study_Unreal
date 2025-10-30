// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IHResultUI.h"
#include <Components/TextBlock.h>
#include "../Util/IHDefine.h"
#include "../Instance/TPS_GameInstance.h"

void UIHResultUI::NativeConstruct()
{
	Super::NativeConstruct();

	UTPS_GameInstance* tpsGameInstace = Cast< UTPS_GameInstance>(GetGameInstance());

	if (tpsGameInstace == nullptr)
		return;

	if (WinText)
	{
		FString Text;
		EWinningTeam winTeam = tpsGameInstace->lastWinTeamType;

		if (winTeam == EWinningTeam::Win_RED)
		{
			Text = TEXT("RED Win !!");
		}

		else if (winTeam == EWinningTeam::Win_Blue)
		{
			Text = TEXT("Blue Win !!");
		}

		else if (winTeam == EWinningTeam::Draw)
		{
			Text = TEXT("Draw");
		}

		WinText->SetText(FText::FromString(Text));
	}
}
