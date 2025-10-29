// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHBullet.generated.h"

UCLASS()
class MYIHGAME_API AIHBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

public:
	// 총알 제거 함수
	void Die();

public:
	// 발사체의 이동을 담당할 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "IHGame|Weapon")
	class UProjectileMovementComponent* MovementComp;
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "IHGame|Weapon")
	class USphereComponent* CollisionComp;
	// 외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "IHGame|Weapon")
	class UStaticMeshComponent* BodyMeshComp;

	// 총알 속도
	UPROPERTY(EditAnywhere, Category = "IHGame|Weapon")
	float Speed = 5000;

	UPROPERTY(EditDefaultsOnly, Category = "MY|BulletTrail")
	TObjectPtr<class UNiagaraSystem> TrailEffect;

	UPROPERTY(EditDefaultsOnly, Category = "MY|BulletTrail")
	TObjectPtr<class UParticleSystem> HittedEffect;

	// 총알을 삭제시키는 타이머
	FTimerHandle DeathTimer;

	UFUNCTION()
	void OnBulletOverlapWith(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBulletHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);



#if WITH_EDITOR
	// 액터의 특정 속성을 수정하면 호출되는 이벤트 함수
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
