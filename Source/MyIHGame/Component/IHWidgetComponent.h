// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "IHWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYIHGAME_API UIHWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;/** Collect all the PSO precache data used by the static mesh component */
	
};
