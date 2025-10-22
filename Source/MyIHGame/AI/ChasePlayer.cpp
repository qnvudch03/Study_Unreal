// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ChasePlayer.h"
#include "../Enemy/IHEnemyController.h"
#include "../Enemy/IHBoss.h"
#include "ChasePlayer.h"

UChasePlayer::UChasePlayer()
{
	NodeName = "Set Speed Chase";
}

EBTNodeResult::Type UChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIHEnemyController* AIController = Cast<AIHEnemyController>(OwnerComp.GetAIOwner());
	AIHBoss* Boss = Cast< AIHBoss>(AIController->GetPawn());

	if (AIController == nullptr || Boss == nullptr)
		return EBTNodeResult::Failed;

	Boss->UpdateWalkSpeed(Boss->ChaseSpeed);

	return EBTNodeResult::Succeeded;
}
