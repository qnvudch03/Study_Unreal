// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHWeapon.generated.h"

UCLASS()
class MYIHGAME_API AIHWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 충돌체크를 위한 캡슐 컴포넌트
	UPROPERTY(VisibleAnywhere, Category="IHGame");
	class USphereComponent* CollisionComp;

	// 비주얼적인 총기 메시 스켈레탈 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "IHGame");
	class USkeletalMeshComponent* WeaponMesh;

	// 총기의 최대 사정거리
	UPROPERTY(EditAnywhere, Category = "IHGame|Value");
	float MaxDistance = 5000;

	UPROPERTY(EditAnywhere, Category = "MY|Value")
	float CrosshairSpreadMAx = 5.0f;

	UPROPERTY(EditAnywhere, Category = "MY|Value")
	float CrosshairSpreadMin = 2.0f;

	UPROPERTY(EditAnywhere, Category = "MY|Value")
	float CrosshairSpreadFire = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MY|FPS")
	float AttackSpeed = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MY|FPS")
	float damage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MY|FireAnimation")
	TObjectPtr<UAnimationAsset> weaponFireAnimation;

	void PlayFireAnimation();

	FVector GetMuzzleFlashLocation();
};
