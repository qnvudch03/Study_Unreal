// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Util/IHDefine.h"
#include "TPS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API ATPS_PlayerController : public APlayerController
{
	GENERATED_BODY()

	void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION(Client, Reliable)
	void Client_ShowGameResult(EWinningTeam winningTeam);

	UFUNCTION(Client, Reliable)
	void Client_MatchState(float startTime, float matchTime);

	float LevelStartTime = 0;
	float LevelMatchTime = 0;

	UFUNCTION(Server, Reliable)
	void Server_OnPlayerNameAssine(ATPS_PlayerController* controller, const FString& name);

	UFUNCTION(NetMulticast, Reliable)
	void Client_BroadCastUserName(ATPS_PlayerController* controller, const FString& name);

	UFUNCTION(Client, Reliable)
	void Client_ReturnToMenu();

	virtual void PostSeamlessTravel() override;

	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float TimeOfClientRequest, float ServerTime);

	float ClientServerDelta = 0;
	float GetServerTime();

	UPROPERTY(EditAnywhere)
	float TimeSyncFrequence = 5.0f;

	float LastTimeSynce = 0;

	/*UFUNCTION(Server, Reliable)
	void Server_PlayerReady();*/
};
