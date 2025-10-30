// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IHUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHUserWidget : public UUserWidget
{
	GENERATED_BODY()

	void UpdateHpBar(class ATPS_PlayerController* PlayerController);
	void UpdateRemainGameTime(class ATPS_PlayerController* PlayerController);

public:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HPText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPProgress;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimeRemain;

	uint32 CountdownTimeSec = 0;
};
