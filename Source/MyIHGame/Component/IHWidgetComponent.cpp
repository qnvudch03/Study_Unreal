// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/IHWidgetComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "IHWidgetComponent.h"

void UIHWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//항상 카메라를 쳐다보게
	APlayerController* PlayerController =  UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PlayerController)
		return;

	FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	FVector WidgetLocation = GetComponentLocation();

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);

	SetWorldRotation(LookAt);
}

void UIHWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* world = GetWorld())
	{
		OwnerPlayer = world->GetFirstLocalPlayerFromController();
	}
}
