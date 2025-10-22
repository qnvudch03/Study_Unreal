// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "IHBTTask_FindNextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHBTTask_FindNextPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UIHBTTask_FindNextPatrolPoint();

private:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
