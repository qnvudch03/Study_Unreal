// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/IHPlayerBaseComponent.h"
#include "IHPlayerFireComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYIHGAME_API UIHPlayerFireComponent : public UIHPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UIHPlayerFireComponent();
	virtual void BeginPlay() override;

	//@TODO 카메라 충돌체크를 위해 Tick 처리
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;

private:
	// 에임 오프셋값 갱신
	void UpdateAimOffset(float DeltaTime);

public:

	UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	TObjectPtr<class UNiagaraSystem> BulletEffectFactory;

	// 총알 궤적 파티클
	UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	TObjectPtr<class UNiagaraSystem> BeamParticelFactory;

	// 탄흔 머티리얼
	UPROPERTY(EditDefaultsOnly, Category = "IHGame|Weapon")
	class UMaterialInterface* BulletDecalMaterial;

	UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	FVector DecalSize = FVector(10.0f, 10.0f, 10.0f);

	UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	float DecalLifetime = 10.f;

	// 책에서 알려준 캐스케이드 파티클 저장
	//UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	//class UParticleSystem* BulletEffectFactory;

	// Actor를 상속받아서 생성한 BP 타입 연결 테스트용.
	UPROPERTY(EditDefaultsOnly, Category = "IHGame|Weapon")
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	// 시작할때 들고있어야 하는 무기 BP 
	UPROPERTY(EditDefaultsOnly, Category = "IHGame|Weapon")
	TSubclassOf<class AIHWeapon> StartWeapon; // BP_Rifle이다.

	// 진짜 스폰된 무기를 저장할 변수
	UPROPERTY()
	AIHWeapon* Weapon;	// 현재 들고있는 무기

	// 에임 오프셋 관련 변수
	float AimOffset_Yaw = 0;
	float AimOffset_Pitch = 0;
	float AimOffset_StartYaw = 0;	// 차이점을 계산할 기준 Yaw값


	UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	class USoundBase* HitSound;

	UPROPERTY()
	class UCameraComponent* CamComp;


	// 카메라vs캐릭터 충돌체크 거리
	UPROPERTY(EditAnywhere, Category = IHGame)
	float CameraThreshold = 80;
	// 카메라가 캐릭터와 너무 가까우면 캐릭터를 숨김처리
	void CheckCameraVisible();

	// ---------------- 입력 -----------------//
	// 총알 발사 처리함수
	void InputFire(const struct FInputActionValue& InputValue);
	// --------------------------------------//
};
