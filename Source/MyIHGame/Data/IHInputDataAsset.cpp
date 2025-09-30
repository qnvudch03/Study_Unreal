// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/IHInputDataAsset.h"
#include "Util/MyIHGame.h" // 여기는 여러분 PRINT_LOG가 있는 파일을 인클루드하세요.

const UInputAction* UIHInputDataAsset::FindInputActionByTag(const FGameplayTag& InputTag) const
{
    // 모든 배열을 순회하면서 Tag와 맞는 데이터를 찾는다.
    for (const FIHInputAction& Action : InputActions)
    {
        // 동일한 태그가 있는지 확인
        if (Action.InputAction && Action.InputTag == InputTag)
        {
            return Action.InputAction;
        }
    }

    // 못찾으면.... 데이터 연결을 안한거니깐, 로그같은걸 출력해서 예외상황을 알수있게 한다.
    PRINT_LOG(TEXT("Can't Find InputAction : [%s]"), *InputTag.ToString());

    return nullptr;
}
