// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestSubscribeActor.generated.h"

UCLASS()
class MYIHGAME_API ATestSubscribeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestSubscribeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 방송 시청 시작
	UFUNCTION(BlueprintCallable)	// BindUObject or AddUFunction 반드시 필요. 
	void StartWatch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ATestYoutuberActor* Youtuber;

};
