// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/IHPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Weapon/IHBullet.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/IHEnemyFSM.h"
#include "Util/IHDefine.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/IHPlayerAnimInstance.h"
#include "Component/IHPlayerMoveComponent.h"
#include "Component/IHPlayerFireComponent.h"
#include "Util/MyIHGame.h"
#include "Data/IHDataSubsystem.h"
#include "Data/IHInputDataAsset.h"
#include "../Interface/TPS_Interactable.h"
#include "../Test/IHColorBox.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AIHPlayer::AIHPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 업데이트가 불필요한 액터들은 Tick 을 호출하지 않을수도 있다.
	PrimaryActorTick.bCanEverTick = true;

	// 스켈레탈메시
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 캡슐의 위치와 메시의 위치를 맞춰준다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//NewObject
	//SpawnActor
	// CreateDefaultSubobject : 컴포넌트 생성시 엔진에 요청하는 함수

	// SpringArm 컴포넌트 붙이기 (카메라 충돌 체크 기능 활용을 위해)
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	//SpringArmComp->SetRelativeLocation(FVector(0, 70, 90));
	SpringArmComp->SocketOffset = FVector(0, 70, 90);
	SpringArmComp->TargetArmLength = 300;
	// 컨트롤러의 회전값을 따라서 SpringArm도 회전한다.
	SpringArmComp->bUsePawnControlRotation = true;

	// Camera 컴포넌트 붙이기
	CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	CamComp->SetupAttachment(SpringArmComp);
	// 실제 카메라는 SpringArm에 붙어서 따라다니기 때문에, 카메라 자체는 컨트롤러의 회전값을 따라가지 않도록 설정
	CamComp->bUsePawnControlRotation = false;

	// 카메라와 캐릭터의 회전 싱크를 맞춘다. 1인칭시점
	bUseControllerRotationYaw = true;

	// 2단 점프
	JumpMaxCount = 2;

	MoveComp = CreateDefaultSubobject<UIHPlayerMoveComponent>(TEXT("MoveComp"));
	FireComp = CreateDefaultSubobject<UIHPlayerFireComponent>(TEXT("FireComp"));
}

void AIHPlayer::Interact_Server_Implementation()
{
	if (InteractableActor)
	{
		ITPS_Interactable::Execute_Interact(InteractableActor, this);
	}

	else if (MoveComp->CurrentHoldColorBox)
	{
		MoveComp->Multicast_PickDropColorBox(MoveComp->CurrentHoldColorBox, nullptr);
	}
}

void AIHPlayer::GetColorBox(AIHColorBox* PickedBox)
{

}

// Called when the game starts or when spawned
void AIHPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (UNetDriver* ND = GetWorld()->GetNetDriver())
	{
		ND->RelevantTimeout = 1.0f;
	}
	
	APlayerController* MyPlayerController = Cast<APlayerController>(Controller);
	if (MyPlayerController)
	{
		auto SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MyPlayerController->GetLocalPlayer());
		if (SubSystem)
		{
			// 싱글톤 GameInstance 가져오기
			UGameInstance* GameInstance = GetWorld()->GetGameInstance();
			UIHDataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UIHDataSubsystem>();
			if (DataSubsystem == nullptr)
				return;

			UIHInputDataAsset* InputData = DataSubsystem->InputDataAsset;
			if (InputData == nullptr)
				return;

			SubSystem->AddMappingContext(InputData->IMC_TPS, 0);
		}
	}

	Hp = InitialHp;

	UpdateCharacterStat(1);
}

void AIHPlayer::SprintStart_Server_Implementation()
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->SprintSpeed;
	}

	SprintStart_Multicast(GetCharacterStat()->SprintSpeed);
}

void AIHPlayer::SprintEnd_Server_Implementation()
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->WalkSpeed;
	}

	SprintEnd_Multicast(GetCharacterStat()->WalkSpeed);
}

void AIHPlayer::SprintStart_Multicast_Implementation(float NewSpeed)
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

void AIHPlayer::SprintEnd_Multicast_Implementation(float NewSpeed)
{
	if (GetCharacterStat())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

// Called every frame
void AIHPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(this);

	TArray<AActor*> IgnoreList;
	IgnoreList.Add(this);

	if (MoveComp->CurrentHoldColorBox)
	{
		IgnoreList.Add(MoveComp->CurrentHoldColorBox);
	}

	float SphereRadius = 50.0f;
	FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 150.0f;
	FVector EndLocation = StartLocation + GetActorForwardVector() * 500.0f;

	auto IsHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(), 
		StartLocation, 
		EndLocation, 
		SphereRadius, 
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic), 
		false, IgnoreList,
		EDrawDebugTrace::ForOneFrame, 
		HitResult, true
	);


	if (IsHit && HitResult.GetActor()->GetClass()->ImplementsInterface(UTPS_Interactable::StaticClass()))
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, SphereRadius, 12, FColor::Magenta, false, 1.0f);
		InteractableActor = HitResult.GetActor();
	}

	else
	{
		InteractableActor = nullptr;
	}
}

// Called to bind functionality to input
void AIHPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		OnInputBindingDelegate.Broadcast(PlayerInput);
		//MoveComp->SetupInputBinding(PlayerInput);
		//FireComp->SetupInputBinding(PlayerInput);
	}
}

FString AIHPlayer::GetLocalRoleString()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		return FString(TEXT("Local : Authority"));
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		return FString(TEXT("Local : AutonomousProxy"));
	}
	else if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		return FString(TEXT("Local : SimulatedProxy"));
	}

	return FString();
}

FString AIHPlayer::GetRemoteRoleString()
{
	if (GetRemoteRole() == ROLE_Authority)
	{
		return FString(TEXT("Remote : Authority"));
	}
	else if (GetRemoteRole() == ROLE_AutonomousProxy)
	{
		return FString(TEXT("Remote : AutonomousProxy"));
	}
	else if (GetRemoteRole() == ROLE_SimulatedProxy)
	{
		return FString(TEXT("Remote : SimulatedProxy"));
	}
	return FString();
}

void AIHPlayer::UpdateCharacterStat(int32 CharacterLevel)
{
	if (CharacterDataTable)
	{
		TArray< FCharacterStat*> CharacterStatRows;
		//CharacterDataTable->GetAllRows<FCharacterStat>(TEXT("Character"), CharacterStatRows);

		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UIHDataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UIHDataSubsystem>();
		if (DataSubsystem == nullptr)
			return;

		DataSubsystem->CharacterDataTable->GetAllRows<FCharacterStat>(TEXT("Character"), CharacterStatRows);


		if (CharacterStatRows.Num() > 0)
		{
			const auto NewCharacterLevel = FMath::Clamp(CharacterLevel, 1, CharacterStatRows.Num());

			CharacterStat = CharacterStatRows[NewCharacterLevel - 1];

			bool isRunning = (CharacterStat->WalkSpeed < GetCharacterMovement()->MaxWalkSpeed);

			isRunning ? (GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->SprintSpeed) :
						(GetCharacterMovement()->MaxWalkSpeed = GetCharacterStat()->WalkSpeed);

		}
	}
}

void AIHPlayer::OnHitEvent()
{
	PRINT_LOG(TEXT("Damaged !!!!!"));
	Hp--;
	if (Hp <= 0)
	{
		PRINT_LOG(TEXT("Player is dead!"));
		OnGameOver();
	}
}

void AIHPlayer::OnGameOver_Implementation()
{
	// 게임 오버 시 일시 정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}
