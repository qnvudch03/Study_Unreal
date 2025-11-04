// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Util/IHDefine.h"
#include "TPS_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnXpChanged, int32, NewXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLvChagned, int32, NewLv);

/**
 * 
 */
UCLASS()
class MYIHGAME_API ATPS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddXp(int32 value);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	int32 SkinIndex = -1;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Character_XP", ReplicatedUsing = "OnRep_Xp")
	int Xp = 0;

	UFUNCTION()
	void OnRep_Xp();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Character_Lv", ReplicatedUsing = "OnRep_Lv")
	int Lv = 1;

	UFUNCTION()
	void OnRep_Lv();

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnXpChanged OnXpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnLvChagned OnLvChagned;

public:


	UPROPERTY(Replicated, VisibleAnywhere, Category = "Character Type")
	ECharacterType CharacterType;


protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void OnRep_PlayerName() override;

public:
	UPROPERTY(Replicated)
	ETeamType Team = ETeamType::FreeForAll;
};
