// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSGame/PlayerBaseComponent.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYUNREALPROJECT_API UWeaponComponent : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	void SetupInputBinding(class UEnhancedInputComponent* playerInput) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	//Gun
	/*UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	class USkeletalMeshComponent* GunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionFire;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	class UStaticMeshComponent* SniperGunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionSwitchWeapon;*/

	UPROPERTY(VisibleAnywhere, Category = "MY|FollowCamera")
	class UCameraComponent* FollowCamera = nullptr;

	void InputFire(const struct FInputActionValue& inputValue);
	void InputSwitchWeapon(const struct FInputActionValue& inputValue);

	bool bIsSniperMode = false;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionFire;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionSwitchWeapon;


	UPROPERTY(EditDefaultsOnly, Category = "MY|CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "MY|GunSound")
	class USoundWave* BuletSound;

	UPROPERTY(EditDefaultsOnly, Category = "MY|GunFire")
	class UNiagaraSystem* BulleftEffectFactory;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	TSubclassOf<class ABullet> bulletFactory;

	///
		
};
