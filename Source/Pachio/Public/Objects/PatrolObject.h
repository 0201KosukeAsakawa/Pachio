// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/ColorTargetTypes.h"
#include "PatrolObject.generated.h"


class UBoxComponent;
class UObjectColorComponent;
class UCameraHandlerComponent;
UCLASS()
class PACHIO_API APatrolObject : public AColorReactiveObject
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。移動可能オブジェクトの初期化を行う
     */
    APatrolObject();

public:
    /**
     * @brief 毎フレーム呼び出される更新処理
     *
     * @param DeltaTime 前フレームからの経過時間
     */
    virtual void Tick(float DeltaTime) override;

private: // オーバーラップイベント

    /**
     * @brief 足エリアにアクターが侵入したときに呼ばれるオーバーラップ開始イベント
     *
     * @param OverlappedComp 侵入が検出されたコンポーネント
     * @param OtherActor 接触したアクター
     * @param OtherComp 接触したアクターのコンポーネント
     * @param OtherBodyIndex ボディインデックス
     * @param bFromSweep スイープによる衝突かどうか
     * @param SweepResult 衝突の詳細情報
     */
    UFUNCTION()
    void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    /**
     * @brief 足エリアからアクターが離れたときに呼ばれるオーバーラップ終了イベント
     *
     * @param OverlappedComp 離脱が検出されたコンポーネント
     * @param OtherActor 離脱したアクター
     * @param OtherComp 離脱したアクターのコンポーネント
     * @param OtherBodyIndex ボディインデックス
     */
    UFUNCTION(BlueprintCallable)
    void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /** @brief ビート（音楽同期）検出時に呼ばれる処理 */
    UFUNCTION()
    void OnBeatDetected();

    /** @brief オブジェクトの移動処理
     *  @param DeltaTime 前フレームからの経過時間
     */
    void Movement(float DeltaTime);

    /** @brief 移動条件やパトロールチェックを行う */
    void Check();

private:

    /** @brief 足判定用のボックスコンポーネント */
    UPROPERTY()
    UBoxComponent* FootTrigger;

    /** @brief 接触しているアクター一覧 */
    UPROPERTY()
    TArray<AActor*> AttachedActors;

    /** @brief 子オブジェクト配列 */
    UPROPERTY(EditAnywhere)
    TArray<AActor*> Child;

    // パトロール関連
    /** @brief パトロールポイント一覧 */
    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<FVector> PatrolPoints;

    /** @brief 現在のパトロールポイントインデックス */
    int32 CurrentPatrolIndex = 0;

    // 移動関連
    /** @brief 1ステップの移動距離 */
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveStepSize = 0.0f;

    /** @brief パトロール到達判定半径 */
    UPROPERTY(EditAnywhere, Category = "Patrol")
    float AcceptanceRadius = 50.f;

    /** @brief 移動にかける時間（秒） */
    float MoveDuration = 0.2f;

    /** @brief 移動経過時間 */
    float MoveElapsedTime = 0.0f;

    /** @brief 移動開始位置 */
    FVector MoveStartLocation;

    /** @brief 移動目標位置 */
    FVector MoveTargetLocation;

    /** @brief 現在のビート再生回数 */
    float PlayCount = 0;

    /** @brief 移動するビートカウント間隔 */
    UPROPERTY(EditAnywhere)
    float BeatCount = 0;

    /** @brief ステップ移動を使用するか */
    UPROPERTY(EditAnywhere)
    bool bUseStepMove = true;

    /** @brief 現在移動中かどうか */
    bool bIsMoving = false;

    /** @brief 移動可能かどうか */
    bool bCanMove = true;
};
