// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../Util/IHDefine.h"
#include "TPS_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UTPS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void BeginLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	UFUNCTION()
	void EndLoadMap(UWorld* InLoadeWorld);

	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void HideLoadingScreen();

	void Init() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTeamMatch = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MY|UserID")
	FString PlayerName;

	UFUNCTION(BlueprintCallable)
	void ExitGame();

	UFUNCTION(BlueprintCallable)
	void StartGame(bool TeamMatch, const FString& InURL);

	TMap<FUniqueNetIdRepl, bool> ConnectedPlayerState;
	void MakeConnectedAllPlayerState();

	bool IsAllPlayerReady();

	void SetPlayerReady(FUniqueNetIdRepl uniqueID);
	void RemovePlayerReady(FUniqueNetIdRepl uniqueID);

	//UPROPERTY()
	//UUserWidget* LoadingWidget;

	TSharedPtr<SWidget> LoadingWidget;

	EWinningTeam lastWinTeamType = EWinningTeam::Draw;
	
};
