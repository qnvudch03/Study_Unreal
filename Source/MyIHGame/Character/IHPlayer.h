// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../CharacterStat/CharacterStat.h"
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

	UFUNCTION(Server, Reliable)
	void SprintStart_Server();

	virtual void SprintStart_Server_Implementation();

	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();

	virtual void SprintEnd_Server_Implementation();


	UFUNCTION(NetMulticast, Reliable)
	void SprintStart_Multicast(float NewSpeed);

	virtual void SprintStart_Multicast_Implementation(float NewSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void SprintEnd_Multicast(float NewSpeed);

	virtual void SprintEnd_Multicast_Implementation(float NewSpeed);


	UFUNCTION(Server, Reliable)
	void Interact_Server();

	virtual void Interact_Server_Implementation();

	void GetColorBox(class AIHColorBox* PickedBox);




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

	void UpdateCharacterStat(int32 CharacterLevel);

	FORCEINLINE FCharacterStat* GetCharacterStat() const { return CharacterStat; }

public:
	UPROPERTY(VisibleAnywhere, Category = "IHGame|Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame|Camera")
	class UCameraComponent* CamComp;


	// 컴포넌트들
	UPROPERTY(VisibleAnywhere, Category = "IHGame")
	class UIHPlayerMoveComponent* MoveComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame")
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

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MY|Character Data", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable;

	//struct FCharacterStat* CharacterStat;
	FCharacterStat* CharacterStat;

	UPROPERTY()
	AActor* InteractableActor;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MY|Stealth", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;
};
