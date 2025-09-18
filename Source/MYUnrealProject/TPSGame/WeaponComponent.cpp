// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/WeaponComponent.h"
#include <InputActionValue.h>
#include <EnhancedInputComponent.h>
#include <Kismet/GameplayStatics.h>
#include "PlayerAnimInstance.h"
#include <Camera/CameraComponent.h>
#include <Camera/CameraShakeBase.h>
#include <Sound/SoundWave.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>
#include "Define.h"
#include "Bullet.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	{
		ConstructorHelpers::FObjectFinder<USoundWave> GunFireSound(TEXT("/Script/Engine.SoundWave'/Game/Assets/FPWeapon/Sniper/Rifle.Rifle'"));
		if (GunFireSound.Object)
		{
			BuletSound = GunFireSound.Object;
		}
	}

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	FollowCamera = Me->GetComponentByClass<UCameraComponent>();
	// ...
	
}

void UWeaponComponent::SetupInputBinding(UEnhancedInputComponent* playerInput)
{
	playerInput->BindAction(ActionFire, ETriggerEvent::Triggered, this, &UWeaponComponent::InputFire);
	playerInput->BindAction(ActionSwitchWeapon, ETriggerEvent::Triggered, this, &UWeaponComponent::InputSwitchWeapon);
}

void UWeaponComponent::InputFire(const FInputActionValue& inputValue)
{

	{
		auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StartCameraShake(CameraShake);
	}

	{
		auto anim = Cast<UPlayerAnimInstance>(Me->GetMesh()->GetAnimInstance());

		if (anim)
		{
			anim->PlayAttackAnim();
		}
	}

	UGameplayStatics::PlaySound2D(GetWorld(), BuletSound);

	if (bIsSniperMode)
	{
		FVector startPos = FollowCamera->GetComponentLocation();
		FVector endPos = startPos + FollowCamera->GetForwardVector() * 5000;

		FHitResult hitInfo;

		FCollisionQueryParams params;

		params.AddIgnoredActor(Me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_weaponTrace, params);

		if (bHit)
		{
			auto hitComp = hitInfo.GetComponent();

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				BulleftEffectFactory,
				hitInfo.Location,
				FRotator::ZeroRotator
			);

			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				FVector dir = (endPos - startPos).GetSafeNormal();

				FVector force = dir * hitComp->GetMass() * 500000;

				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamagedProcess();
			}
		}

	}

	else
	{
		FTransform firePosition = Me->GunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}

}

void UWeaponComponent::InputSwitchWeapon(const FInputActionValue& inputValue)
{
	if (Me->GunMeshComp->IsVisible())
	{
		Me->GunMeshComp->SetVisibility(false);
		Me->SniperGunMeshComp->SetVisibility(true);

		Me->ShowSniperModeUI();

		bIsSniperMode = true;

		return;
	}

	else if (Me->SniperGunMeshComp->IsVisible())
	{
		Me->SniperGunMeshComp->SetVisibility(false);
		Me->GunMeshComp->SetVisibility(true);

		Me->HideSniperModeUI();

		bIsSniperMode = false;

		return;
	}
}
