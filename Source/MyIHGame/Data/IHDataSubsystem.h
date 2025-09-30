// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IHDataSubsystem.generated.h"

/**
 * 게임에서 사용하는 데이터들을 관리하는 클래스
 */
UCLASS()
class MYIHGAME_API UIHDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UIHDataSubsystem();

	UPROPERTY()
	TObjectPtr<class UIHInputDataAsset> InputDataAsset;
	
};
