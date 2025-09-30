// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/IHDataSubsystem.h"
#include "Data/IHInputDataAsset.h"

UIHDataSubsystem::UIHDataSubsystem()
{
	// 기본 디폴트 에셋 설정
	ConstructorHelpers::FObjectFinder<UIHInputDataAsset> TempInputData(
		TEXT("/Script/MyIHGame.IHInputDataAsset'/Game/Data/DA_Input.DA_Input'"));
	if (TempInputData.Succeeded())
	{
		InputDataAsset = TempInputData.Object;
	}
}
