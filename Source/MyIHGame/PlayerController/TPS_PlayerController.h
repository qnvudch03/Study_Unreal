// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

	virtual void OnRep_PlayerState() override;

	UFUNCTION(Server, Reliable)
	void Server_OnPlayerNameAssine(ATPS_PlayerController* controller, const FString& name);

	UFUNCTION(NetMulticast, Reliable)
	void Client_BroadCastUserName(ATPS_PlayerController* controller, const FString& name);

	UFUNCTION(Client, Reliable)
	void Client_ReturnToMenu();
};
