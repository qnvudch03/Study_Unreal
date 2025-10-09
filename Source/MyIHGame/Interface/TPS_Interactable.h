// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPS_Interactable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UTPS_Interactable : public UInterface
{
	GENERATED_BODY()
};

class MYIHGAME_API ITPS_Interactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MY|Interaction", meta = (DisplayName = "Interact"))
	void Interact(class AIHPlayer* CharacterInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MY|Interaction", meta = (DisplayName = "Can Interact"))
	bool CanInteract(class AIHPlayer* CharacterInstigator) const;
};
