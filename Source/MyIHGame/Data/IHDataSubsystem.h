// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../CharacterStat/CharacterStat.h"
#include "IHDataSubsystem.generated.h"

/**
 * 게임에서 사용하는 데이터들을 관리하는 클래스
 */
UCLASS(Blueprintable)
class MYIHGAME_API UIHDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UIHDataSubsystem();

	//블루프린트 버젼만 사용하겠다? Instance 계열을, C++버젼과 블루프린트 버젼까지해서 2개를 만들것
	//SingleTon 계열은 엔진이 알아서 만들어 주는데..그래서 C++버젼이랑, 블루프린트가 존재한다면 2개가 만들어짐
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UPROPERTY()
	TObjectPtr<class UIHInputDataAsset> InputDataAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UDataTable> CharacterDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class USkeletalMesh*> SkinAssetList;
};
