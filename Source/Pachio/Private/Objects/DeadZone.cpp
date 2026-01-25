// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/DeadZone.h"
#include "Interface/StateControllable.h"
#include "Components/BoxComponent.h"
#include "Components/RespawnComponent.h"
#include "GameFramework/Actor.h"

// =======================
// コンストラクタ
// =======================

// Sets default values
ADeadZone::ADeadZone()
{
	// 毎フレームTickを有効化（必要なら無効化しても良い）
	PrimaryActorTick.bCanEverTick = true;

	// 死亡判定用の BoxComponent を作成し、Root に設定
	DeadArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DeadArea"));
	RootComponent = DeadArea;

	// コリジョン設定：
	// ・クエリ（Overlap 判定）のみ有効
	// ・デフォルトでは全チャンネル無視
	// ・Pawn チャンネルのみ Overlap を許可
	DeadArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DeadArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	DeadArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Overlap イベントをバインド
	DeadArea->OnComponentBeginOverlap.AddDynamic(this, &ADeadZone::OverlapDead);
}

// =======================
// ゲーム開始時処理
// =======================

void ADeadZone::BeginPlay()
{
	Super::BeginPlay();
	// 必要ならここで初期化処理
}

// =======================
// 毎フレーム処理
// =======================

void ADeadZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 現状は特に処理なし
}

// =======================
// オーバーラップイベント処理
// =======================

// ゾーンにアクターが侵入したときに呼ばれる
void ADeadZone::OverlapDead(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;


	// RespawnComponent が無い場合は State を Dead に変更
	if (IStateControllable* IS = Cast<IStateControllable>(OtherActor))
	{
		IS->ChangeState(EPlayerStateType::Dead);
	}
	// RespawnComponent を持っている場合はリスポーン処理を実行
	if (URespawnComponent* RespawnComp = OtherActor->FindComponentByClass<URespawnComponent>())
	{
		RespawnComp->RespawnOwner();
		return;
	}


}
