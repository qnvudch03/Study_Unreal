// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 공용의 함수 & Enum & 구조체 내용들을 정의한다.

#define ECC_TRACE_WEAPON ECC_GameTraceChannel1

UENUM(BlueprintType)
enum class ETurnInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Turn Left"),
	ETIP_Right UMETA(DisplayName = "Turn Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning")
};