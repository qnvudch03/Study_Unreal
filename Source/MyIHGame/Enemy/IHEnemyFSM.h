// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IHEnemyFSM.generated.h"


// 사용할 상태 정의
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle UMETA(DisplayName = "Enemy Idle State"),
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYIHGAME_API UIHEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIHEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//UPROPERTY(ReplicatedUsing = "State_Implement", VisibleAnywhere, BlueprintReadOnly, Category = "IHGame|FSM")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame|FSM", ReplicatedUsing = "OnRep_State")
	EEnemyState State = EEnemyState::Idle;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void OnStateChange_Server(EEnemyState NewState);

	virtual void OnStateChange_Server_Implementation(EEnemyState NewState);


	UFUNCTION()
	void OnRep_State();
		
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();


	// 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "IHGame|FSM")
	float IdleDelayTime = 2;
	// 공격 범위
	UPROPERTY(EditAnywhere, Category = "IHGame|FSM")
	float AttackRange = 150.0f;
	// 공격 대기 시간
	UPROPERTY(EditAnywhere, Category = "IHGame|FSM")
	float AttackDelayTime = 2.0f;
	// 피격 대기 시간
	UPROPERTY(EditAnywhere, Category = "IHGame|FSM")
	float DamageDelayTime = 2.0f;
	// 아래로 사라지는 속도
	UPROPERTY(EditAnywhere, Category = "IHGame|FSM")
	float DieSpeed = 50.0f;
	// 공격 가능 시야
	UPROPERTY(EditAnywhere, Category = "IHGame|FSM")
	float AttachSightAngle = 30.0f;

	// 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IHGame|FSM")
	int32 Hp = 3;

	float CurrentTime = 0;

	// 타깃
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame|FSM")
	class AIHPlayer* Target;

	// 사용 중인 애니메이션 블루프린트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MY|AnimInst")
	class UIHEnemyAnimInstance* AnimInst;

	// 오너
	UPROPERTY()
	class AIHEnemy* OwnerCharacter;	//me

	// Enemy를 소유하고 있는 AIController
	UPROPERTY()
	class AAIController* AI;

	// 공격 당함
	void OnDamageProcess();

	// 길 찾기 수행시 랜덤 위치
	FVector RandomPos;
	bool GetRandomPositionInNavMesh(FVector CenterLocation, float Radius, FVector& Dest);
};
