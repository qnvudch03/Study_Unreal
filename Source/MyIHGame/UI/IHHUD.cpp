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

	// 가운데 점 텍스처
	if (CrosshairsCenter)
	{
		DrawCrossHair(CrosshairsCenter, ViewportCenter);
	}

	// 네장의 텍스처
	if (CrosshairsLeft)
	{
		DrawCrossHair(CrosshairsLeft, ViewportCenter);
	}
	if (CrosshairsRight)
	{
		DrawCrossHair(CrosshairsRight, ViewportCenter);
	}
	if (CrosshairsTop)
	{
		DrawCrossHair(CrosshairsTop, ViewportCenter);
	}
	if (CrosshairsBottom)
	{
		DrawCrossHair(CrosshairsBottom, ViewportCenter);
	}
}

void AIHHUD::DrawCrossHair(UTexture2D* Texture, FVector2D Position)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	//@TODO 추후, 에임이 벌어지는 효과 만들때 수정할 예정
	const FVector2D TextureDrawPoint = Position;

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
		FColor::White
	);
}

