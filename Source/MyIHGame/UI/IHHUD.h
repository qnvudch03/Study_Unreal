// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IHHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API AIHHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;
	float ComputeMaxScreenspaceSpreadRadius();

private:
	void DrawCrossHair(UTexture2D* Texture, FVector2D Position, FVector2D Spread);

public:
	// 가운데 중심점
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsCenter;
	 
	// 네장의 상하좌우
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsBottom;

private:
	float CrosshairSpread = 0;

public:
	FORCEINLINE void SetCrosshairSpread(float Spread) { CrosshairSpread = Spread; }
};
