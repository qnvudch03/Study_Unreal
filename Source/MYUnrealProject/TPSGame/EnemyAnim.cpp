// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/EnemyAnim.h"
#include "../EnemyFSM.h"

void UEnemyAnim::OnEndAttackAnimation()
{
	bAttackPlay = false;

	auto FSM = GetOwningActor()->FindComponentByClass< UEnemyFSM>();
	if (FSM)
	{
		FSM->onAttackAnimEnd();
	}
}

void UEnemyAnim::OnDieAnimEnd()
{
	auto FSM = GetOwningActor()->FindComponentByClass< UEnemyFSM>();
	if (FSM)
	{
		FSM->OnDie();
	}
}
