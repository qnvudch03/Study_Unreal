// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IHPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, class UEnhancedInputComponent*);

UCLASS()
class MYIHGAME_API AIHPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIHPlayer();

public:
	// 입력 바인딩 델리게이트
	FInputBindingDelegate OnInputBindingDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 네트워크 역할&권한 출력
	// 캐릭터 3개를 스폰할건데, 각 캐릭터마다 역할이 다르다. (서버/클라이언트1/클라이언트2)
	// 코드에서 판단하려면?
	UFUNCTION(BlueprintCallable)
	FString GetLocalRoleString();

	UFUNCTION(BlueprintCallable)
	FString GetRemoteRoleString();

public:
	UPROPERTY(VisibleAnywhere, Category = "IHGame|Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame|Camera")
	class UCameraComponent* CamComp;


	// 컴포넌트들
	UPROPERTY(VisibleAnywhere, Category = "IHGame")
	class UIHPlayerMoveComponent* MoveComp;
	UPROPERTY(VisibleAnywhere, Category = "IHGame")
	class UIHPlayerFireComponent* FireComp;

	// 현재 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	int32 Hp;
	// 초기 hp 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|Value")
	int32 InitialHp = 10;

	// 피격 당했을 때 처리
	UFUNCTION(BlueprintCallable, Category = "IHGame")
	void OnHitEvent();

	// 게임 오버될 때 호출될 함수
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IHGame")
	void OnGameOver();

	// 총 바꿀 때 호출되는 이벤트 함수
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Health)
	void OnUsingGrenade(bool isGrenade);
};
