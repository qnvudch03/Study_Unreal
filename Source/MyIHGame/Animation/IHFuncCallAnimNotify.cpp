// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/IHFuncCallAnimNotify.h"

TArray<FName> UIHFuncCallAnimNotify::GetCallableFunctionNames() const
{
    TArray<FName> Out;

    if (TargetClass)
    {
        //UClass* TargetClass = UAnimInstance::StaticClass();
        for (TFieldIterator<UFunction> It(TargetClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
        {
            UFunction* Fn = *It;
            const bool bCallable = Fn->HasAnyFunctionFlags(FUNC_BlueprintCallable | FUNC_BlueprintEvent);
            if (!bCallable) continue;

            Out.Add(Fn->GetFName());
        }
    }

    Out.Sort(FNameLexicalLess());
    return Out;
}

void UIHFuncCallAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    if (!TargetClass) return;
    
    //UObject* Target = TargetClass->IsChildOf<UAnimInstance>() ? Cast<UObject>(MeshComp->GetAnimInstance()) : MeshComp->GetOwner();

    UObject* Target = nullptr;

    if (TargetClass->IsChildOf<UAnimInstance>())
    {
        Target = Cast<UObject>(MeshComp->GetAnimInstance());
    }

    else if (TargetClass->IsChildOf<UActorComponent>())
    {
        AActor* TargetActor = MeshComp->GetOwner();
        if (TargetActor)
        {
            Target = TargetActor->FindComponentByClass(TargetClass);
        }
    }

    else
    {
        Target = MeshComp->GetOwner();
    }

    if (!Target) return;

    // 캐시 없으면 한 번만 탐색
    if (CachedFunction == nullptr || CachedFunction->GetFName() != FunctionName)
    {
        CachedFunction = Target->FindFunction(FunctionName);
    }

    if (CachedFunction == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AnimNotify] '%s' Can't Find Function"), *FunctionName.ToString());
        CachedFunction = nullptr;
        return;
    }

    // 함수 호출
    Target->ProcessEvent(CachedFunction, nullptr);
}

#if WITH_EDITOR
void UIHFuncCallAnimNotify::PostEditChangeProperty(FPropertyChangedEvent& E)
{
    Super::PostEditChangeProperty(E);
    
    if (E.Property && E.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UIHFuncCallAnimNotify, FunctionName))
    {
        CachedFunction = nullptr;
    }
}

void UIHFuncCallAnimNotify::ValidateAssociatedAssets()
{
    Super::ValidateAssociatedAssets();
    if (FunctionName.IsNone())
    {
        //FMessageLog("PIE").Warning(LOCTEXT("NoFn", "UIHFuncCallAnimNotify: FunctionName이 비어 있습니다."));
    }
}
#endif