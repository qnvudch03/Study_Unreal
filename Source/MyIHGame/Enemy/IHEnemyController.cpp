// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHEnemyController.h"
#include <Kismet/GameplayStatics.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AISenseConfig_Hearing.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AISense_Sight.h>
#include <Perception/AISense_Hearing.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "../Character/IHPlayer.h"
#include "IHPatrolPoint.h"

AIHEnemyController::AIHEnemyController()
{
	// AI Perception
	PawnSense = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AISense"));

	// Sight Config (시각)
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1600.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	SightConfig->SetMaxAge(5.0f); // 5초정도 기억하기
	PawnSense->ConfigureSense(*SightConfig);	// 시각 등록

	// Hearing Config (청각)
	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->HearingRange = 800.f;
	PawnSense->ConfigureSense(*HearingConfig);	// 청각 등록

	// 메인 감각 설정
	PawnSense->SetDominantSense(SightConfig->GetSenseImplementation());

	// Controller에 감각을 설정한다.
	SetPerceptionComponent(*PawnSense);
}

void AIHEnemyController::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIHPatrolPoint::StaticClass(), PatrolPoints);

	if (GetAIPerceptionComponent())
	{
		GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AIHEnemyController::OnTargetPerceptionUpdated);
	}
}

void AIHEnemyController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/*if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	if (PawnSense)
	{
		PawnSense->OnTargetPerceptionUpdated.AddDynamic(this, &AIHEnemyController::OnTargetPerceptionUpdated);
	}*/
	
}

void AIHEnemyController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* blackBoardComp = GetBlackboardComponent();

	if (blackBoardComp == nullptr)
		return;

	AIHPlayer* TargetPlayer = Cast<AIHPlayer>(Actor);

	if (Stimulus.WasSuccessfullySensed())
	{
		if (TargetPlayer)
		{
			blackBoardComp->SetValueAsBool(BBKey_bCanSeePlayer, true);
			blackBoardComp->SetValueAsObject(BBKey_Player, TargetPlayer);

			return;
		}
	}

	/*blackBoardComp->SetValueAsBool(BBKey_bCanSeePlayer, false);
	blackBoardComp->SetValueAsObject(BBKey_Player, NULL);*/
}
