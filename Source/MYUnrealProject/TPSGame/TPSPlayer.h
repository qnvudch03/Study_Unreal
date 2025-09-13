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

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArmComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* InputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ActionLook = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ActionJump = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ActionMove = nullptr;

	//Gun
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	class USkeletalMeshComponent* GunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ActionFire;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
	class UStaticMeshComponent* SniperGunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ActionSwitchWeapon;

	void InputFire(const struct FInputActionValue& inputValue);
	void InputSwitchWeapon(const struct FInputActionValue& inputValue);

	UPROPERTY(EditAnywhere, Category = Gun)
	TSubclassOf<class UUserWidget> SniperUIFactory;

	UPROPERTY()
	class UUserWidget* SniperModeUI = nullptr;

	UPROPERTY(EditAnywhere, Category = Gun)
	TSubclassOf<class UUserWidget> HandGunUIFactory;

	UPROPERTY()
	class UUserWidget* HandGUnModeUI = nullptr;

	bool bIsSniperMode = false;

	UPROPERTY(EditDefaultsOnly, Category = GunFire)
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
