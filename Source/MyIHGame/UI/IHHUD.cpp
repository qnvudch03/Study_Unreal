// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IHHUD.h"

void AIHHUD::DrawHUD()
{
	Super::DrawHUD();

	// 크로스헤어 그리기
	// 항상 화면의 가운데 기준으로 그려져야한다.
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController == nullptr)	// nullptr 체크
		return;

	// 현재 화면 사이즈 구해오기
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// 센터 위치 구하기
	FVector2D ViewportCenter(ViewportSizeX / 2.f, ViewportSizeY / 2.f);

	float SpreadScaled = ComputeMaxScreenspaceSpreadRadius();

	// 가운데 점 텍스처
	if (CrosshairsCenter)
	{
		FVector2D Spread(0, 0);
		DrawCrossHair(CrosshairsCenter, ViewportCenter, Spread);
	}

	// 네장의 텍스처
	if (CrosshairsLeft)
	{
		FVector2D Spread(-SpreadScaled, 0);
		DrawCrossHair(CrosshairsLeft, ViewportCenter, Spread);
	}
	if (CrosshairsRight)
	{
		FVector2D Spread(SpreadScaled, 0);
		DrawCrossHair(CrosshairsRight, ViewportCenter, Spread);
	}
	if (CrosshairsTop)
	{
		FVector2D Spread(0, -SpreadScaled);
		DrawCrossHair(CrosshairsTop, ViewportCenter, Spread);
	}
	if (CrosshairsBottom)
	{
		FVector2D Spread(0, SpreadScaled);
		DrawCrossHair(CrosshairsBottom, ViewportCenter, Spread);
	}
}

float AIHHUD::ComputeMaxScreenspaceSpreadRadius()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && playerController->PlayerCameraManager)
	{
		float FOV = playerController->PlayerCameraManager->GetFOVAngle();

		int32 ViewportSizeX, ViewportSizeY;
		playerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

		float ScreenDistance = (ViewportSizeY * 0.5f) / FMath::Tan(FMath::DegreesToRadians(FOV * 0.5f));

		float ScreenSpreadRadius = ScreenDistance * FMath::Tan(FMath::DegreesToRadians(CrosshairSpread * 0.5f));
		return ScreenSpreadRadius;
	}

	return 0;
}

void AIHHUD::DrawCrossHair(UTexture2D* Texture, FVector2D Position, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint = Position + Spread;

	DrawTexture(
		Texture,
		TextureDrawPoint.X - TextureWidth / 2,	// 텍스처의 센터 기준으로 보정
		TextureDrawPoint.Y - TextureHeight / 2,	// 텍스처의 센터 기준으로 보정
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}

//void AIHHUD::DrawCrossHair(UTexture2D* Texture, FVector2D Position)
//{
//	const float TextureWidth = Texture->GetSizeX();
//	const float TextureHeight = Texture->GetSizeY();
//
//	//@TODO 추후, 에임이 벌어지는 효과 만들때 수정할 예정
//	const FVector2D TextureDrawPoint = Position;
//
//	DrawTexture(
//		Texture,
//		TextureDrawPoint.X - TextureWidth / 2,	// 텍스처의 센터 기준으로 보정
//		TextureDrawPoint.Y - TextureHeight / 2,	// 텍스처의 센터 기준으로 보정
//		TextureWidth,
//		TextureHeight,
//		0.f,
//		0.f,
//		1.f,
//		1.f,
//		FColor::White
//	);
//}

