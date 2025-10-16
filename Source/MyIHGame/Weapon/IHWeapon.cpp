// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/IHWeapon.h"
#include "Components/SphereComponent.h"

// Sets default values
AIHWeapon::AIHWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));

	// 시작 무기는 가지고 태어날거기때문에 충돌체를 끄고 시작
	CollisionComp->SetCollisionProfileName(TEXT("NoCollision"));
	CollisionComp->SetSphereRadius(64);

	// 충돌을 루트로 설정
	RootComponent = CollisionComp;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(CollisionComp);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AIHWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIHWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIHWeapon::PlayFireAnimation()
{
	if (weaponFireAnimation)
	{
		WeaponMesh->PlayAnimation(weaponFireAnimation, false);
	}
}

FVector AIHWeapon::GetMuzzleFlashLocation()
{
	if (WeaponMesh)
	{
		FVector MuzzleFlashLocation =  WeaponMesh->GetSocketLocation("MuzzleFlash");
		return MuzzleFlashLocation;
	}

	else
	{
		return FVector::Zero();
	}
	
}

