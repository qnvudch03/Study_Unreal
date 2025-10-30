// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TPS_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API ATPS_GameMode : public AGameMode
{
	GENERATED_BODY()

private:
	//int32 SkinCount = 0;

public:
	UPROPERTY(EditDefaultsOnly)
	float WaitTIme = 10.0f;

	float CountdownTime = 10.0f;

	float LevelStartTIme = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float LevelMatchTimeSec = 120.f;

protected:
	virtual void BeginPlay() override;

	virtual bool ReadyToStartMatch_Implementation() override;

	virtual void HandleMatchHasStarted() override;

public:
	ATPS_GameMode();

	virtual void Tick(float DeltaTIme) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION(BlueprintCallable, Category = "MY|Minions")
	void AlertMinions(class AActor* AlertInstigator, const FVector& Location, const float Radius);

	virtual void RestartPlayer(AController* NewPlyaer) override;

	void PlayerRespawn(ACharacter* DeathCharacter);

public:
	virtual void Logout(AController* Exiting) override;
	
};
