// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IHUserWidget.h"
#include "../Character/IHPlayer.h"
#include "../Component/HealthComponent.h"
#include <Components/TextBlock.h>
#include <Components/ProgressBar.h>
#include "../Character/IHPlayer.h"
#include "../PlayerController/TPS_PlayerController.h"

void UIHUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ATPS_PlayerController* PlayerController = Cast<ATPS_PlayerController>(GetOwningPlayer());
	if (!PlayerController)
	{
		return;
	}

	UpdateHpBar(PlayerController);

	UpdateRemainGameTime(PlayerController);
}

void UIHUserWidget::UpdateHpBar(ATPS_PlayerController* PlayerController)
{

	AIHPlayer* PlayerCharacter = Cast<AIHPlayer>(PlayerController->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}

	if (!PlayerCharacter->healthComp)
	{
		return;
	}

	if (HPText)
	{
		FString Text = FString::Printf(TEXT("%d %d"), PlayerCharacter->healthComp->currentHP, PlayerCharacter->healthComp->maxHP);
		HPText->SetText(FText::FromString(Text));
	}

	if (HPProgress)
	{
		float ratio = (float)PlayerCharacter->healthComp->currentHP / PlayerCharacter->healthComp->maxHP;
		HPProgress->SetPercent(ratio);
	}
}

void UIHUserWidget::UpdateRemainGameTime(class ATPS_PlayerController* PlayerController)
{
	if (TimeRemain == nullptr)
		return;

	float LeftTime = PlayerController->LevelMatchTime - (PlayerController->GetServerTime() - PlayerController->LevelStartTime);

	if (LeftTime <= 0)
	{
		LeftTime = 0;
	}

	uint32 SecondLeft = FMath::CeilToInt(LeftTime);

	if (SecondLeft != CountdownTimeSec)
	{
		int32 Minute = FMath::FloorToInt(LeftTime / 60.0f);
		int32 Second = LeftTime - (Minute * 60);

		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minute, Second);
		TimeRemain->SetText(FText::FromString(CountdownText));

		CountdownTimeSec = SecondLeft;
	}

}


