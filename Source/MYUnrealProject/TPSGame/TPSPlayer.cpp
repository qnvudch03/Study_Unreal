// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGame/TPSPlayer.h"
#include "Define.h"
#include "Bullet.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputActionValue.h>
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include <Animation/AnimBlueprint.h>
#include <Animation//AnimInstance.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerAnimInstance.h"
#include <Sound/SoundBase.h>
#include "PlayerMoveComp.h"
#include "WeaponComponent.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> Ue4_Manny(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/Animation/SK_Mannequin.SK_Mannequin'"));
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/Assets/Animation/ABP_PlayerAnimBP.ABP_PlayerAnimBP_C'"));

	if (Ue4_Manny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Ue4_Manny.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, 0));

		if (AnimBP.Class)
		{
			GetMesh()->SetAnimClass(AnimBP.Class);
		}
	}

	{
		SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringAram"));
		SpringArmComp->SetupAttachment(RootComponent);
		SpringArmComp->SetRelativeRotation(FRotator(0, 0, -90));
		SpringArmComp->SocketOffset = FVector(0, 70, 90);
		SpringArmComp->TargetArmLength = 500.0f;
		SpringArmComp->bUsePawnControlRotation = true;

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
		FollowCamera->SetupAttachment(SpringArmComp);
		FollowCamera->bUsePawnControlRotation = false;

		bUseControllerRotationYaw = true;
	}

	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
		ConstructorHelpers::FObjectFinder<UStaticMesh> SniperGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/Assets/FPWeapon/Sniper/sniper11.sniper11'"));

		if (GunMesh.Object)
		{
			GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
			GunMeshComp->SetSkeletalMesh(GunMesh.Object);
			GunMeshComp->SetupAttachment(GetMesh(),TEXT("hand_rSocket"));

			GunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
			GunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));
		}

		if (SniperGunMesh.Object)
		{
			SniperGunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunMeshComp"));
			SniperGunMeshComp->SetStaticMesh(SniperGunMesh.Object);
			SniperGunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
			SniperGunMeshComp->SetRelativeLocation(FVector(-42, 7, 1));
			SniperGunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));
			SniperGunMeshComp->SetRelativeScale3D(FVector(0.15f));

			SniperGunMeshComp->SetVisibility(false);
		}
	}

	//Components
	{
		MoveComp = CreateDefaultSubobject<UPlayerMoveComp>(TEXT("MoveComp"));
		WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComp"));
	}

	JumpMaxCount = 2;

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	if (PlayerController)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	SniperModeUI = CreateWidget(GetWorld(), SniperUIFactory);
	SniperModeUI->AddToViewport(0);
	SniperModeUI->SetVisibility(ESlateVisibility::Collapsed);

	HandGUnModeUI = CreateWidget(GetWorld(), HandGunUIFactory);
	HandGUnModeUI->AddToViewport(0);
	HandGUnModeUI->SetVisibility(ESlateVisibility::Visible);


}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* pc = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	MoveComp->SetupInputBinding(pc);
	WeaponComp->SetupInputBinding(pc);

	if (pc)
	{
		

		pc->BindAction(Ia_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		pc->BindAction(Ia_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);

		//pc->BindAction(Ia_Run, ETriggerEvent::Triggered, this, &ATPSPlayer::InputRun);
	}

}

void ATPSPlayer::ShowSniperModeUI()
{
	if (SniperModeUI == nullptr)
	{
		return;
	}

	SniperModeUI->SetVisibility(ESlateVisibility::Visible);
	HandGUnModeUI->SetVisibility(ESlateVisibility::Collapsed);

	FollowCamera->SetFieldOfView(30);
}

void ATPSPlayer::HideSniperModeUI()
{
	if (SniperModeUI == nullptr)
	{
		return;
	}

	SniperModeUI->SetVisibility(ESlateVisibility::Collapsed);
	HandGUnModeUI->SetVisibility(ESlateVisibility::Visible);

	FollowCamera->SetFieldOfView(90);
}

void ATPSPlayer::InputRun()
{
	auto movement = GetCharacterMovement();

	if (movement->MaxWalkSpeed > walkSpeed)
	{
		movement->MaxWalkSpeed = walkSpeed;
	}

	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}










