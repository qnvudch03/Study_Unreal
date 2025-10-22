// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHBTTask_FindNextPatrolPoint.h"
#include <BehaviorTree/BlackboardComponent.h>
#include "IHEnemyController.h"
#include "IHBoss.h"

UIHBTTask_FindNextPatrolPoint::UIHBTTask_FindNextPatrolPoint()
{
	NodeName = "Find Patrol Point";
}

EBTNodeResult::Type UIHBTTask_FindNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AIHEnemyController* AIController = Cast<AIHEnemyController>(OwnerComp.GetAIOwner());

	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (AIController->PatrolPoints.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	AActor* CurrPatrolPoint = AIController->PatrolPoints[AIController->CurrentPatrolIndex];
	if (CurrPatrolPoint == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AIHBoss* Boss = Cast< AIHBoss>(AIController->GetPawn());
	if (Boss == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Boss->UpdateWalkSpeed(Boss->PatrolSpeed);

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), CurrPatrolPoint->GetActorLocation());

	AIController->CurrentPatrolIndex = (AIController->CurrentPatrolIndex + 1) % AIController->PatrolPoints.Num();


	return EBTNodeResult::Succeeded;
}
