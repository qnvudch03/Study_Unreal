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

public:
	UPROPERTY(VisibleAnywhere,Category = "MY|Camera")
	class USpringArmComponent* SpringArmComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "MY|Camera")
	class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Input")
	class UInputMappingContext* InputMapping = nullptr;

	//Component
	UPROPERTY(VisibleDefaultsOnly, Category = "MY|MoveComp")
	class UPlayerMoveComp* MoveComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "MY|WeaponComp")
	class UWeaponComponent* WeaponComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "MY|Gun")
	TSubclassOf<class UUserWidget> SniperUIFactory;

	UPROPERTY(EditAnywhere, Category = "MY|Gun")
	TSubclassOf<class UUserWidget> HandGunUIFactory;

	UPROPERTY()
	class UUserWidget* HandGUnModeUI = nullptr;

	UPROPERTY()
	class UUserWidget* SniperModeUI = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	class USkeletalMeshComponent* GunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "MY|Gun")
	class UStaticMeshComponent* SniperGunMeshComp;

	

};
