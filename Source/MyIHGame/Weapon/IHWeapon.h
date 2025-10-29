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
	// Ãæµ¹Ã¼Å©¸¦ À§ÇÑ Ä¸½¶ ÄÄÆ÷³ÍÆ®
	UPROPERTY(VisibleAnywhere, Category="IHGame");
	class USphereComponent* CollisionComp;

	// ºñÁÖ¾óÀûÀÎ ÃÑ±â ¸Þ½Ã ½ºÄÌ·¹Å» ÄÄÆ÷³ÍÆ®
	UPROPERTY(VisibleAnywhere, Category = "IHGame");
	class USkeletalMeshComponent* WeaponMesh;

	// ÃÑ±âÀÇ ÃÖ´ë »çÁ¤°Å¸®
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Value")
	float damage_Min = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Value")
	float damage_InnerRadious = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MY|Value")
	float damage_OuterRadious = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MY|FireAnimation")
	TObjectPtr<UAnimationAsset> weaponFireAnimation;

	void PlayFireAnimation();

	FVector GetMuzzleFlashLocation();

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Test(AActor* OverlappedActor, AActor* OtherActor);

	void Equip();
	void UnEquip();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MY|Value")
	TSubclassOf<class AIHBullet> Bullet;

	void Fire(FVector startPos, FVector randomDir);

	void FireBullet(FVector startPos, FVector randomDir);
	void FireLineTrace(FVector startPos, FVector randomDir);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IHGame|Weapon")
	TObjectPtr<class UNiagaraSystem> BulletEffectFactory;

	// ÃÑ¾Ë ±ËÀû ÆÄÆ¼Å¬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IHGame|Weapon")
	TObjectPtr<class UNiagaraSystem> BeamParticelFactory;

	// ÅºÈç ¸ÓÆ¼¸®¾ó
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Weapon")
	class UMaterialInterface* BulletDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "IHGame|Weapon")
	FVector DecalSize = FVector(10.0f, 10.0f, 10.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IHGame|Weapon")
	float DecalLifetime = 10.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IHGame|Weapon")
	class USoundBase* HitSound;
};
