// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

#define CALLINFO ( FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")") )

#define PRINT_CALLINFO() UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO)
#define PRINT_LOG(x) UE_LOG(LogTemp, Log, TEXT(x))

/**
 * 
 */
UCLASS()
class MYUNREALPROJECT_API ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
};
