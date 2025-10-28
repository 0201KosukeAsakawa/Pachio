// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ConfigTriggerZone.h"
#include "Player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CameraHandlerComponent.h"

// =======================
// コンストラクタ
// =======================

// Sets default values
AConfigTriggerZone::AConfigTriggerZone()
{
	// このアクターを毎フレーム Tick させる（不要ならオフにして最適化可能）
	PrimaryActorTick.bCanEverTick = true;

	// Box Collision を生成して Root に設定
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	// オーバーラップ開始イベントをバインド
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AConfigTriggerZone::OnOverlapBegin);
}

// =======================
// 開始時処理
// =======================

// Called when the game starts or when spawned
void AConfigTriggerZone::BeginPlay()
{
	Super::BeginPlay();
	// 初期化が必要ならここで行う
}

// =======================
// オーバーラップイベント処理
// =======================

void AConfigTriggerZone::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp, // この BoxComponent
	AActor* OtherActor,                  // 侵入したアクター
	UPrimitiveComponent* OtherComp,      // 侵入したアクターのコンポーネント
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// Null チェック
	if (OtherActor == nullptr)
		return;

	// プレイヤーキャラクターかどうか判定
	APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
	if (player == nullptr)
		return;

	// カメラ制御コンポーネントを取得
	UCameraHandlerComponent* cameraHandle = player->GetComponentByClass<UCameraHandlerComponent>();
	if (cameraHandle == nullptr)
		return;

	// すでに同じカメラ設定が適用済みか判定
	if (!cameraHandle->IsParameterMatch(GridSize, ZBaffer, CameraViewType))
	{
		// 異なる設定ならカメラ設定を適用
		cameraHandle->ApplyCameraSettings(GridSize, ZBaffer, CameraViewType);
	}
}
