// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

// 가독성을 위해서 네임스페이스로 감싼다
namespace IHGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Look);	//2개로 분리된사람은 두개로 선언
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Run);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Fire);
};

