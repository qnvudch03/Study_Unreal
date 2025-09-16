// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../EnemyFSM.h"
#include "TPSPlayer.generated.h"


UCLASS()
class MYUNREALPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void ShowSniperModeUI();

	UFUNCTION()
	void HideSniperModeUI();

	UPROPERTY(EditDefaultsOnly, Category = "MY|Anim")
	class UAnimBlueprint* PlayerAnimBlueprint;

	UPROPERTY(EditAnywhere, Category = "MY|PlayerSetting")
	float walkSpeed = 200;

	UPROPERTY(EditAnywhere, Category = "MY|PlayerSetting")
	float runSpeed = 600;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* Ia_Run;

	void InputRun();

private:
	UPROPERTY(VisibleAnywhere, Category = "MY|Camera")
	class USpringArmComponent* SpringArmComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "MY|Camera")
	class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputMappingContext* InputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionLook = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionJump = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionMove = nullptr;

	//Gun
	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	class USkeletalMeshComponent* GunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionFire;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	class UStaticMeshComponent* SniperGunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputAction* ActionSwitchWeapon;

	void InputFire(const struct FInputActionValue& inputValue);
	void InputSwitchWeapon(const struct FInputActionValue& inputValue);

	UPROPERTY(EditAnywhere, Category = "MY|Gun")
	TSubclassOf<class UUserWidget> SniperUIFactory;

	UPROPERTY()
	class UUserWidget* SniperModeUI = nullptr;

	UPROPERTY(EditAnywhere, Category = "MY|Gun")
	TSubclassOf<class UUserWidget> HandGunUIFactory;

	UPROPERTY()
	class UUserWidget* HandGUnModeUI = nullptr;

	bool bIsSniperMode = false;

	UPROPERTY(EditDefaultsOnly, Category = "MY|GunFire")
	class UNiagaraSystem* BulleftEffectFactory;

	///

	void Look(const struct FInputActionValue& inputValue);

	FVector Direction;
	//float MovingSpeed = 100;
	void Move(const struct FInputActionValue& inputValue);
	void Moving();

	void InputJump(const struct FInputActionValue& inputValue);

	float CameraMinDistance = 35;
	void CheckCameraVisible();

};
