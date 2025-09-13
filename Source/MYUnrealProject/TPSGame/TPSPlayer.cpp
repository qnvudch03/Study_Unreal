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
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> Ue5_Manny(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));

	if (Ue5_Manny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Ue5_Manny.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, 0));
	}

	{
		SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringAram"));
		SpringArmComp->SetupAttachment(RootComponent);
		//SpringArmComp->SetRelativeRotation(FRotator(0, 0, 90));
		SpringArmComp->SocketOffset = FVector(0, 70, 90);
		SpringArmComp->TargetArmLength = 500.0f;

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
		FollowCamera->SetupAttachment(SpringArmComp);
	}

	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
		ConstructorHelpers::FObjectFinder<UStaticMesh> SniperGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/Assets/FPWeapon/Sniper/sniper11.sniper11'"));

		if (GunMesh.Object)
		{
			GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
			GunMeshComp->SetSkeletalMesh(GunMesh.Object);
			GunMeshComp->SetupAttachment(GetMesh());

			GunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
		}

		if (SniperGunMesh.Object)
		{
			SniperGunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunMeshComp"));
			SniperGunMeshComp->SetStaticMesh(SniperGunMesh.Object);
			SniperGunMeshComp->SetupAttachment(GetMesh());
			SniperGunMeshComp->SetRelativeLocation(FVector(-22, 55, 150));
			SniperGunMeshComp->SetRelativeScale3D(FVector(0.15f));

			SniperGunMeshComp->SetVisibility(false);
		}
	}

	JumpMaxCount = 2;

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);

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

	if (pc)
	{
		pc->BindAction(ActionLook, ETriggerEvent::Triggered, this, &ATPSPlayer::Look);
		pc->BindAction(ActionMove, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		pc->BindAction(ActionJump, ETriggerEvent::Triggered, this, &ATPSPlayer::InputJump);
		pc->BindAction(ActionFire, ETriggerEvent::Triggered, this, &ATPSPlayer::InputFire);
		pc->BindAction(ActionSwitchWeapon, ETriggerEvent::Triggered, this, &ATPSPlayer::InputSwitchWeapon);
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

void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{
	if (bIsSniperMode)
	{
		FVector startPos = FollowCamera->GetComponentLocation();
		FVector endPos = startPos + FollowCamera->GetForwardVector() * 5000;

		FHitResult hitInfo;

		FCollisionQueryParams params;

		params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_weaponTrace, params);

		if (bHit)
		{
			auto hitComp = hitInfo.GetComponent();

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				BulleftEffectFactory,
				hitInfo.Location,
				FRotator::ZeroRotator
			);

			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				FVector dir = (endPos - startPos).GetSafeNormal();

				FVector force = dir * hitComp->GetMass() * 500000;

				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamagedProcess();
			}
		}

	}

	else
	{
		FTransform firePosition = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	
}

void ATPSPlayer::InputSwitchWeapon(const FInputActionValue& inputValue)
{
	if (GunMeshComp->IsVisible())
	{
		GunMeshComp->SetVisibility(false);
		SniperGunMeshComp->SetVisibility(true);

		ShowSniperModeUI();

		bIsSniperMode = true;

		return;
	}

	else if (SniperGunMeshComp->IsVisible())
	{
		SniperGunMeshComp->SetVisibility(false);
		GunMeshComp->SetVisibility(true);

		HideSniperModeUI();

		bIsSniperMode = false;

		return;
	}
}

void ATPSPlayer::Look(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	AddControllerYawInput(value.X);

	FRotator CurrentRot = FollowCamera->GetRelativeRotation();
	CurrentRot.Pitch += value.Y;

	if (CurrentRot.Pitch > 25)
	{
		CurrentRot.Pitch = 25;
	}

	else if (CurrentRot.Pitch < -15)
	{
		CurrentRot.Pitch = -15;
	}

	FollowCamera->SetRelativeRotation(CurrentRot);

	CheckCameraVisible();

}

void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	Direction.X = value.X;
	Direction.Y = value.Y;

	Moving();
}

void ATPSPlayer::Moving()
{
	FRotator Temp1 = GetControlRotation();
	FTransform Temp2 = FTransform(GetControlRotation());
	FVector Temp3 = FTransform(GetControlRotation()).TransformVector(Direction);


	Direction = FTransform(GetControlRotation()).TransformVector(Direction);
	AddMovementInput(Direction);
	Direction = FVector::ZeroVector;
}

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::CheckCameraVisible()
{
	FVector DistVec = FollowCamera->GetComponentLocation() - GetActorLocation();
	float Distance = DistVec.Size();
	if (Distance < CameraMinDistance)
	{
		GetMesh()->SetVisibility(false);
	}
	else
	{
		GetMesh()->SetVisibility(true);
	}
}

