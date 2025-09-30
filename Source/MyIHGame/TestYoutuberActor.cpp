// Fill out your copyright notice in the Description page of Project Settings.


#include "TestYoutuberActor.h"

// Sets default values
ATestYoutuberActor::ATestYoutuberActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestYoutuberActor::BeginPlay()
{
	Super::BeginPlay();

	// 딜레이
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer,
		FTimerDelegate::CreateLambda([this]()
			{
				// 함수 1개짜리
				//OnAirStart.ExecuteIfBound();	// 예외처리가 들어간 버전

				// 멀티캐스트용 델리게이트
				OnAirStart.Broadcast();
			})
	, 2, false);
	//OnAirStart.ExecuteIfBound();	// 예외처리가 들어간 버전
}

// Called every frame
void ATestYoutuberActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

