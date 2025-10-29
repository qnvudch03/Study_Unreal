// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_GameMode.h"
#include "../Player/TPS_PlayerState.h"
#include "../Character/TPS_Character.h"
#include "../PlayerController/TPS_PlayerController.h"
#include "../Enemy/TPS_Minion.h"
#include "../UI/IHHUD.h"
#include <GameFramework/PlayerStart.h>
#include <Kismet/GameplayStatics.h>
#include "../Data/IHDataSubsystem.h"
#include "../Player/TPS_PlayerState.h"
#include "../Component/IHPlayerFireComponent.h"
#include "../Weapon/IHWeapon.h"
#include "../Character/IHPlayer.h"
#include "../Game/TPS_GameState.h"
#include "../Game/IHPlayerStart.h"
#include <GameFramework/PlayerStart.h>
#include <EngineUtils.h>
#include "../Instance/TPS_GameInstance.h"
ATPS_GameMode::ATPS_GameMode()
{
	GameStateClass = ATPS_GameMode::StaticClass();

	PlayerStateClass = ATPS_PlayerState::StaticClass();
	PlayerControllerClass = ATPS_PlayerController::StaticClass();

	HUDClass = AIHHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));

	bUseSeamlessTravel = true;

	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GameStateClass = ATPS_GameState::StaticClass();
}

AActor* ATPS_GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	Super::ChoosePlayerStart_Implementation(Player);

	// 팀이 정해지지 않았다고 한다면, 팀을 정해서 적당한 Player Start 위치를 찾아서 리턴한다.
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	// 개인전이 디폴트 
	ETeamType TeamType = ETeamType::FreeForAll;

	UTPS_GameInstance* IHGameInstance = Cast<UTPS_GameInstance>(GetGameInstance());
	ATPS_GameState* IHGameState = GetGameState<ATPS_GameState>();
	if (IHGameState == nullptr)
	{
		return nullptr;
	}

	// 팀전일 경우에만 팀 설정
	if (IHGameInstance->bTeamMatch)
	{
		ATPS_PlayerState* IHPlayerState = Player->GetPlayerState<ATPS_PlayerState>();
		if (IHPlayerState)
		{
			// 팀이 정해진적 없고, 첫 스폰이다. 그러면 팀을 정해주자.
			if (IHPlayerState->Team == ETeamType::FreeForAll)
			{
				// Red, Blue 팀원이 누구인지 정보는 어디에 있을까?
				// GameState에 있으면 좋을것 같습니다.
				if (IHGameState->BlueTeam.Num() >= IHGameState->RedTeam.Num())
				{
					IHPlayerState->Team = ETeamType::Team_Red;
					IHGameState->RedTeam.AddUnique(IHPlayerState);
					TeamType = ETeamType::Team_Red;
				}
				else
				{
					IHPlayerState->Team = ETeamType::Team_Blue;
					IHGameState->BlueTeam.AddUnique(IHPlayerState);
					TeamType = ETeamType::Team_Blue;
				}
			}
			else
			{
				// 본인이 알고있는 팀의 위치에 스폰시킨다.
				TeamType = IHPlayerState->Team;
			}
		}
	}


	APlayerStart* FoundPlayerStart = nullptr;

	// 모든 PlayerStart 찾아서 골고루 배치.
	// 팀별로 PlayerStart가 구분이되어 있다.

	TArray<APlayerStart*> UnOccupiedStartPoints[3]; // Blue, Red, FreeForAll
	TArray<APlayerStart*> OccupiedStartPoints[3];   // Blue, Red, FreeForAll

	// 비어있는 플레이어 스타트를 팀별로 분류작업
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		AIHPlayerStart* PlayerStart = Cast<AIHPlayerStart>(*It);

		if (PlayerStart == nullptr)
			continue;

		FVector ActorLocation = PlayerStart->GetActorLocation();
		const FRotator ActorRotation = PlayerStart->GetActorRotation();

		// 로비에서 정해진 폰을 스폰시킬꺼라... 디폴트 폰은 아니다.
		if (!World->EncroachingBlockingGeometry(Player->GetPawn(), ActorLocation, ActorRotation))
		{
			UnOccupiedStartPoints[(int)PlayerStart->GetTeam()].Add(PlayerStart);
		}
		else if (World->FindTeleportSpot(Player->GetPawn(), ActorLocation, ActorRotation))
		{
			OccupiedStartPoints[(int)PlayerStart->GetTeam()].Add(PlayerStart);
		}
	}

	// 분류작업 끝난뒤에, 실제로 어디에 스폰시킬지 정하면된다.
	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedStartPoints[(int)TeamType].Num() > 0)
		{
			FoundPlayerStart = UnOccupiedStartPoints[(int)TeamType][FMath::RandRange(0, UnOccupiedStartPoints[(int)TeamType].Num() - 1)];
		}
		else if (OccupiedStartPoints[(int)TeamType].Num() > 0)
		{
			FoundPlayerStart = OccupiedStartPoints[(int)TeamType][FMath::RandRange(0, OccupiedStartPoints[(int)TeamType].Num() - 1)];
		}
	}
	return FoundPlayerStart;
}

void ATPS_GameMode::AlertMinions(AActor* AlertInstigator, const FVector& Location, const float Radius)
{
	TArray<AActor*> Minions;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPS_Minion::StaticClass(), Minions);

	if (Minions.Num() > 0)
	{
		for (const auto Minion : Minions)
		{
			if (Minion == AlertInstigator)
				continue;

			auto Distance = FVector::Distance(AlertInstigator->GetActorLocation(), Minion->GetActorLocation());

			if (Distance < Radius)
			{
				static_cast<ATPS_Minion*>(Minion)->GoToLocation(Location);
			}
		}
	}
}

void ATPS_GameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	/*if (NewPlayer)
	{
		UIHDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UIHDataSubsystem>();

		if (DataSubsystem == nullptr)
		{
			return;
		}

		AIHPlayer* Player = Cast< AIHPlayer>(NewPlayer->GetPawn());
		ATPS_PlayerState* playerState = Cast< ATPS_PlayerState>(Player->GetPlayerState());

		if (playerState->SkinIndex == -1)
		{
			playerState->SkinIndex = SkinCount;
			Player->ApplyCharacterSkin();
		}

		SkinCount = (SkinCount + 1) % DataSubsystem->SkinAssetList.Num();
	}*/
}

void ATPS_GameMode::PlayerRespawn(ACharacter* DeathCharacter)
{
	if (DeathCharacter == nullptr)
		return;

	AController* Controller = DeathCharacter->GetController();
	int32 RandIndex = 0;
	TArray<AActor*> PlayerStatActors;

	if (Controller)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStatActors);

		RandIndex = FMath::RandRange(0, PlayerStatActors.Num() - 1);
	}

	if (DeathCharacter)
	{
		AIHPlayer* deadCharacter = Cast<AIHPlayer>(DeathCharacter);
		deadCharacter->FireComp->Weapon->Destroy();

		//DeathCharacter->Reset();
		DeathCharacter->Destroy();

	}

	RestartPlayerAtPlayerStart(Controller, PlayerStatActors[RandIndex]);
}
