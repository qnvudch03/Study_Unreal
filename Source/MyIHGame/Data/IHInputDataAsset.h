// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "IHInputDataAsset.generated.h"


// 구조체 : UInputActor을 GameplayTag와 묶어서 처리하는 데이터
USTRUCT()
struct FIHInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInputAction> InputAction;
};


/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHInputDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Player에 있는 IMC_TPS 가져오기
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* IMC_TPS;

	// 여러개의 InputAction을 관리하기위해 TArray로 변경
	UPROPERTY(EditDefaultsOnly)
	TArray<FIHInputAction> InputActions;
	
	// 게임플레이 태그에 맞는 입력 액션을 찾아준다.
	const class UInputAction* FindInputActionByTag(const FGameplayTag& InputTag) const;

};
