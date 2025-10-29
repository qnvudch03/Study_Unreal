// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "../Util/IHDefine.h"
#include "IHPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API AIHPlayerStart : public APlayerStart
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class UBillboardComponent> TypeSprite;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTexture2D> EditorTeamIconBlue;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTexture2D> EditorTeamIconRed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ETeamType TeamType = ETeamType::FreeForAll;

public:

	AIHPlayerStart(const FObjectInitializer& ObjectInitializer);

	ETeamType GetTeam() { return TeamType; }

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

#endif
};
