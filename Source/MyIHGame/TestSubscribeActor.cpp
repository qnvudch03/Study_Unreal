// Fill out your copyright notice in the Description page of Project Settings.


#include "TestSubscribeActor.h"
#include "TestYoutuberActor.h"
#include "Util/MyIHGame.h"

// Sets default values
ATestSubscribeActor::ATestSubscribeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestSubscribeActor::BeginPlay()
{
	Super::BeginPlay();

	// 좋아하는 유투버가 설정이 되어있으면 구독 설정
	if (Youtuber)
	{
		// 멀티캐스트 (여러명이 등록할수 있다)
		//Youtuber->OnAirStart.BindUObject(this, &ATestSubscribeActor::StartWatch);
		
		// 1번 버전
		//Youtuber->OnAirStart.AddUObject(this, &ATestSubscribeActor::StartWatch);
		// 2번 버전
		//Youtuber->OnAirStart.AddUFunction(this, TEXT("StartWatch"));

		Youtuber->OnAirStart.AddDynamic(this, &ATestSubscribeActor::StartWatch);
	}
}

// Called every frame
void ATestSubscribeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestSubscribeActor::StartWatch()
{
	PRINT_LOG(TEXT("Youtuber Start OnAir, Watch!!"));
}

