// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/IHEnemy.h"
#include "Enemy/IHEnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationInvokerComponent.h"

// Sets default values
AIHEnemy::AIHEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/Book/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
	}

	// EnemyFSM 컴포넌트 추가
	FSM = CreateDefaultSubobject<UIHEnemyFSM>(TEXT("FSM"));

	// 네비게이션 인보커를 디폴트로 생성한다.
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavMeshInvoker"));
	NavInvoker->SetGenerationRadii(500, 800);

	// 이동방향으로 캐릭터를 회전시킨다.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Animations/ABP_Enemy.ABP_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	// 월드에 배치될때 자동 AI Controller로 조종
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AIHEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIHEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

