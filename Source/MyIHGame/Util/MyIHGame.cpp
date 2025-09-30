// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyIHGame.h"
#include "Modules/ModuleManager.h"
#include "Kismet/GameplayStatics.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MyIHGame, "MyIHGame" );
DEFINE_LOG_CATEGORY(IHGame);

void PrintLogWithRole(const UObject* WorldContextObject, FString Text, FLinearColor TextColor, float Duration)
{
	UKismetSystemLibrary::PrintString(WorldContextObject, Text, true, true, TextColor, Duration);
}

void PrintScreenLog(int32 Key, float TimeToDisplay, FColor DisplayColor, FString Text)
{
	GEngine->AddOnScreenDebugMessage(Key, TimeToDisplay, DisplayColor, Text);
}
