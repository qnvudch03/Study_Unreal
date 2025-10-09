// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHEnemyFSM.h"
#include "Enemy/IHEnemy.h"
#include "Character/IHPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Util/MyIHGame.h"
#include "Components/CapsuleComponent.h"
#include "Animation/IHEnemyAnimInstance.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values for this component's properties
UIHEnemyFSM::UIHEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UIHEnemyFSM::BeginPlay()
{
	Super::BeginPlay();
	// ...
	// 월드에서 ATPSPlayer 타깃 찾아오기
	AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIHPlayer::StaticClass());	
	Target = Cast<AIHPlayer>(Actor);

	// 싱글게임이라 PlayerController 한개만 존재
	//APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	//Target = Cast<AIHPlayer>(PlayerController->GetPawn());
	
	// 소유 객체 가져오기
	OwnerCharacter = Cast<AIHEnemy>(GetOwner());
	AnimInst = Cast<UIHEnemyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());

	if (OwnerCharacter)
	{
		// AAIController 할당하기
		AI = Cast<AAIController>(OwnerCharacter->GetController());
	}
	
}


// Called every frame
void UIHEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	FString logMsg = UEnum::GetValueAsString(State);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (State)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UIHEnemyFSM::IdleState()
{
	CurrentTime += GetWorld()->DeltaTimeSeconds;

	// 대기 시간을 초과했다면
	if (CurrentTime > IdleDelayTime)
	{
		State = EEnemyState::Move;
		CurrentTime = 0;

		AnimInst->AnimState = State;

		// 이동 시작전에 랜덤 위치 구해주기
		GetRandomPositionInNavMesh(OwnerCharacter->GetActorLocation(), 500, RandomPos);
	}
}

void UIHEnemyFSM::MoveState()
{
	// 목적지까지 방향 벡터 구하기 : 목적지 - 출발지
	FVector Destination = Target->GetActorLocation();
	FVector Dir = Destination - OwnerCharacter->GetActorLocation();
	
	//OwnerCharacter->AddMovementInput(Dir.GetSafeNormal());
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	// 목적지 길 찾기 경로 데이터 검색
	FPathFindingQuery Query;
	FAIMoveRequest Req;
	Req.SetAcceptanceRadius(3);
	Req.SetGoalLocation(Destination);

	if (AI == nullptr)
		return;
	
	AI->BuildPathfindingQuery(Req, Query);
	
	// 길 찾기 결과 가져오기
	FPathFindingResult r = ns->FindPathSync(Query);
	// 경로 데이터가 있고, 완전한 경로일 경우에만
	if (r.Result == ENavigationQueryResult::Success && r.Path.IsValid() && !r.Path->IsPartial())
	{
		// 길찾기 성공시 타깃쪽으로 이동
		AI->MoveToLocation(Destination);
	}
	else
	{
		// 랜덤 위치로 이동
		auto Result = AI->MoveToLocation(RandomPos);
		if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// 목적지에 도착하면 현재위치 기준으로 새로운 랜덤 위치 가져오기
			GetRandomPositionInNavMesh(OwnerCharacter->GetActorLocation(), 500, RandomPos);
		}
	}


	// 공격 가능한 범위다
	// 뒤쪽에 있을수도 있으니 각도 체크
	float DotValue = FVector::DotProduct(OwnerCharacter->GetActorForwardVector(), Dir.GetSafeNormal());
	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotValue));
	if (Dir.Size() < AttackRange && Angle < AttachSightAngle)
	{
		AI->StopMovement();

		State = EEnemyState::Attack;

		AnimInst->AnimState = State;
		AnimInst->bAttackPlay = true;
		CurrentTime = AttackDelayTime;
		PRINT_LOG(TEXT("Attack(2)!!!!!"));
	}
}

void UIHEnemyFSM::AttackState()
{
	// 목표: 일정 시간에 한 번씩 공격하고 싶다.
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	
	// 공격 타임 시간 만족
	if (CurrentTime > AttackDelayTime)
	{
		// 3. 공격하고 싶다.
		PRINT_LOG(TEXT("Attack!!!!!"));
		CurrentTime = 0;
		AnimInst->bAttackPlay = true;
	}

	if (AnimInst->bAttackPlay == false)
	{
		// 목표: 타깃이 공격 범위를 벗어나면 추적상태
		// 타깃과의 거리 체크
		// 뒤쪽에 있을수도 있으니 각도 체크
		FVector Dir = Target->GetActorLocation() - OwnerCharacter->GetActorLocation();
		float DotValue = FVector::DotProduct(OwnerCharacter->GetActorForwardVector(), Dir.GetSafeNormal());
		float Angle = FMath::RadiansToDegrees(FMath::Acos(DotValue));

		float Distance = FVector::Distance(Target->GetActorLocation(), OwnerCharacter->GetActorLocation());
		if (Distance > AttackRange || Angle > AttachSightAngle)
		{
			State = EEnemyState::Move;
			AnimInst->AnimState = State;

			// 이동할 새로운 랜덤 위치 가져오기
			GetRandomPositionInNavMesh(OwnerCharacter->GetActorLocation(), 500, RandomPos);
		}
	}
}

void UIHEnemyFSM::DamageState()
{
	CurrentTime += GetWorld()->DeltaTimeSeconds;

	// 경과시간이 데미지 처리 시간을 넘었다
	if (CurrentTime > DamageDelayTime)
	{
		State = EEnemyState::Idle;
		CurrentTime = 0;

		AnimInst->AnimState = State;
	}
}

void UIHEnemyFSM::DieState()
{
	if (AnimInst->bDieDone == false)
	{
		return;
	}

	FVector P0 = OwnerCharacter->GetActorLocation();
	FVector vt = FVector::DownVector * DieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	OwnerCharacter->SetActorLocation(P);

	// 2미터 이상 내려왔다면
	if (P.Z < -200.0f)
	{
		// 월드에서 제거
		OwnerCharacter->Destroy();
	}
}

void UIHEnemyFSM::OnDamageProcess()
{
	// 체력 감소
	Hp--;
	if (Hp > 0)
	{
		// 상태를 피격으로 전환
		State = EEnemyState::Damage;

		CurrentTime = 0;

		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		AnimInst->PlayDamageAnim(FName(*sectionName));
	}
	// 그렇지 않다면
	else
	{
		// 상태를 죽음으로 전환
		if (State != EEnemyState::Die)
		{
			State = EEnemyState::Die;
			OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AnimInst->PlayDamageAnim(TEXT("Die"));
		}
	}	
	AnimInst->AnimState = State;
	AI->StopMovement();
}

// 랜덤 위치 가져오기
bool UIHEnemyFSM::GetRandomPositionInNavMesh(FVector CenterLocation, float Radius, FVector& Dest)
{
	if (OwnerCharacter->GetLocalRole() != ENetRole::ROLE_Authority)
		return false;

	auto NS = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	if (NS == nullptr)
		return false;

	FNavLocation Loc;
	bool Result = NS->GetRandomReachablePointInRadius(CenterLocation, Radius, Loc);

	Dest = Loc.Location;
	return Result;
}