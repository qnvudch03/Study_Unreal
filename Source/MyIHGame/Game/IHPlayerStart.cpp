// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IHPlayerStart.h"
#include <Component/IHPlayerBaseComponent.h>
#include <Components/BillboardComponent.h>

AIHPlayerStart::AIHPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    TypeSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("TypeSprite"));
    if (TypeSprite)
    {
        TypeSprite->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
        TypeSprite->SetRelativeLocation(FVector(0, 0, 50));
        TypeSprite->bHiddenInGame = true;
        TypeSprite->SetUsingAbsoluteScale(true);
        TypeSprite->bIsScreenSizeScaled = true;
        TypeSprite->bIsEditorOnly = true;

        TypeSprite->SetupAttachment(RootComponent);
    }
}

void AIHPlayerStart::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AIHPlayerStart, TeamType))
    {
        if (TypeSprite)
        {
            UTexture2D* TargetSprite = nullptr;
            switch (TeamType)
            {
            case ETeamType::Team_Blue:
                TargetSprite = EditorTeamIconBlue;
                break;
            case ETeamType::Team_Red:
                TargetSprite = EditorTeamIconRed;
                break;
            default:
                break;
            }
            TypeSprite->SetSprite(TargetSprite);
        }
    }
}
