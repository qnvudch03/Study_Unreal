// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/IHEnemyAnimInstance.h"
#include "Util/MyIHGame.h"

//@TODO - For TEST
#include "Character/IHPlayer.h"
#include "Enemy/IHEnemyFSM.h"
#include "Enemy/IHEnemy.h"

void UIHEnemyAnimInstance::OnEndAttackAnimation()
{
	bAttackPlay = false;
	PRINT_LOG(TEXT("Attack Reset!"));
}


void UIHEnemyAnimInstance::TEST_OnHitPlayer()
{
	AIHEnemy* Enemy = Cast<AIHEnemy>(TryGetPawnOwner());
	if (Enemy && Enemy->FSM->Target)
	{
		Enemy->FSM->Target->OnHitEvent();
	}
}