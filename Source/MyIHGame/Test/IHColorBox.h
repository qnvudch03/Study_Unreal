// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/TPS_Interactable.h"
#include "IHColorBox.generated.h"

UCLASS()
class MYIHGAME_API AIHColorBox : public AActor, public ITPS_Interactable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHColorBox();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> OwnerCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Cube;

	// 복제할 변수를 선언
	//UPROPERTY(Replicated)	// 블루프린트의 Replicated 옵션과 동일
	//UPROPERTY(ReplicatedUsing = "OnRep_Color")	// RepNotify 옵션과 동일
	FLinearColor Color;

	UFUNCTION()
	void OnRep_Color();

	// RPC
	// 서버에 색상을 바꿔달라고 요구
	UFUNCTION(Server, Reliable)  // 유실되면 큰일나는 중요한 정보다.
	void ServerChangeColor();

	// 서버가 -> 모든 클라이언트한테 색상을 전달 (Multicast)
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastUpdateColor(FLinearColor NewColor);

	// 서버가 -> 소유하고 있는 클라이언트의 함수를 실행
	UFUNCTION(Client, Unreliable)
	void ClientUpdateColor(FLinearColor NewColor);



	// 박스와 겹쳤을떄 호출되는 함수
	UFUNCTION()
	void OnBeginOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	// Owner 박스와 겹쳤을떄 호출되는 함수
	UFUNCTION()
	void OnBeginOverlapSetOwner(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Interact_Implementation(class AIHPlayer* CharacterInstigator);
};
