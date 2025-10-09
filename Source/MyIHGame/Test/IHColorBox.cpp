// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/IHColorBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/IHPlayer.h"
#include "Component/IHPlayerMoveComponent.h"
#include "Net/UnrealNetwork.h"	// 헤더를 인크루드해야 네트워크 사용가능

// Sets default values
AIHColorBox::AIHColorBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetBoxExtent(FVector(150, 150, 150));
	RootComponent = BoxCollision;

	OwnerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OwnerCollision"));
	OwnerCollision->SetBoxExtent(FVector(150, 150, 150));
	OwnerCollision->SetupAttachment(RootComponent);
	OwnerCollision->SetRelativeLocation(FVector(320, 0, 0));

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->SetupAttachment(RootComponent);

	// 복제를 켜야합니다.
	SetReplicates(true);
}

// 해당 변수를 복제해달라고 등록
void AIHColorBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AIHColorBox, Color);
}

// Called when the game starts or when spawned
void AIHColorBox::BeginPlay()
{
	Super::BeginPlay();
	
	// 블루프린트에서 + 아이콘만 누르면 OverlapEvent 바인딩이 되었지만,
	// C++ 코드는 델리게이트 바인딩을 한다.
	// 바인딩은 왠만하면 BeginPlay()
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AIHColorBox::OnBeginOverlapCollision);
	OwnerCollision->OnComponentBeginOverlap.AddDynamic(this, &AIHColorBox::OnBeginOverlapSetOwner);
}

// Called every frame
void AIHColorBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIHColorBox::OnBeginOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AIHPlayer* Player = Cast<AIHPlayer>(OtherActor);
	if (Player)	// 플레이어와 겹쳤다면
	{
		/* RPC 버전 */
		// 서버에 요구
		//if(Player->IsLocallyControlled())  // 내가 조종중인 캐릭터로 체크하던지

		if (OtherActor == GetOwner() || GetOwner() == nullptr) // 소유권에 대한 정보를 체크하던지
		{
			ServerChangeColor();
		}


		/* 변수 복제 버전
		// 서버일때만 랜덤 색상 결정
		if (HasAuthority())
		{
			Color = FLinearColor::MakeRandomColor();

			// 서버는 자동으로 호출안해주니깐, 스스로 챙겨서 호출
			OnRep_Color();
		}
		*/
	}
}

void AIHColorBox::OnBeginOverlapSetOwner(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AIHPlayer* Player = Cast<AIHPlayer>(OtherActor);
	if (Player)	// 플레이어와 겹쳤다면
	{
		SetOwner(Player);
	}
}

void AIHColorBox::Interact_Implementation(AIHPlayer* CharacterInstigator)
{
	if (HasAuthority())
	{
		CharacterInstigator->MoveComp->Multicast_PickDropColorBox(CharacterInstigator->MoveComp->CurrentHoldColorBox, this);
	}
}

void AIHColorBox::OnRep_Color()
{
	// Color 값이 변경되어서 서버로부터 값을 전달받으면, 머티리얼 인스턴스의 색상을 변경한다.
	if (Cube)
	{
		Cube->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Color));
	}
}

// 서버에 색상 바꿔달라고 요구하는 함수
void AIHColorBox::ServerChangeColor_Implementation()
{
	Color = FLinearColor::MakeRandomColor();
	
	MulticastUpdateColor(Color);
}

void AIHColorBox::ClientUpdateColor_Implementation(FLinearColor NewColor)
{
	Color = NewColor;

	// Color 값이 변경되어서 서버로부터 값을 전달받으면, 머티리얼 인스턴스의 색상을 변경한다.
	if (Cube)
	{
		Cube->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Color));
	}
}

void AIHColorBox::MulticastUpdateColor_Implementation(FLinearColor NewColor)
{
	Color = NewColor;

	// Color 값이 변경되어서 서버로부터 값을 전달받으면, 머티리얼 인스턴스의 색상을 변경한다.
	if (Cube)
	{
		Cube->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Color));
	}
}

