// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Util/IHDefine.h"
#include "../Character/TPS_Character.h"
#include "../Character/IHPlayer.h"
#include "IHDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECharacterType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AIHPlayer> BlueprintAsset;

	
};
