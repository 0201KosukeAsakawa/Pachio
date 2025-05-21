// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/DeadZone.h"
#include "Interface/StateControllable.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
ADeadZone::ADeadZone()
{
	PrimaryActorTick.bCanEverTick = true; // 毎フレーム Tick を呼ぶ設定

	// ゴール用の BoxComponent を生成し、ルートとして設定
	DeadArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DeadArea"));
	RootComponent = DeadArea;

	// コリジョン設定：
	// ・物理処理は行わず、オーバーラップ検知のみ行う
	DeadArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// ・全てのチャンネルに対して無視
	DeadArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	// ・Pawn（プレイヤー）とのオーバーラップのみ有効
	DeadArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// オーバーラップイベント登録：プレイヤーが入ったら OnGoalOverlap を呼ぶ
	DeadArea->OnComponentBeginOverlap.AddDynamic(this, &ADeadZone::OverlapDead);

}

// Called when the game starts or when spawned
void ADeadZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeadZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeadZone::OverlapDead(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	IStateControllable* IS = Cast<IStateControllable>(OtherActor);

	if (!IS)
		return;

	IS->ChangeState("Dead");

}
