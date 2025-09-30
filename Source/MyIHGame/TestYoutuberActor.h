// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestYoutuberActor.generated.h"

// 유투버가 방송을 시작할때 함수 포인터를 통해서 알림을 보낸다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyOnAirStart);

UCLASS()
class MYIHGAME_API ATestYoutuberActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestYoutuberActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FMyOnAirStart OnAirStart; // 방송 시작 알람용 함수
};
