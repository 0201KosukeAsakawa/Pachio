// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadZone.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API ADeadZone : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。デッドゾーンアクターの初期化を行う
     */
    ADeadZone();

protected:

    /** @brief プレイヤーやオブジェクトが接触すると死亡判定される範囲 */
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* DeadArea;

    /**
     * @brief ゲーム開始時またはスポーン時に呼ばれる初期化処理
     */
    virtual void BeginPlay() override;

public:
    /**
     * @brief 毎フレーム呼び出される更新処理
     *
     * @param DeltaTime 前フレームからの経過時間
     */
    virtual void Tick(float DeltaTime) override;

private:

    /**
     * @brief デッドゾーンにアクターが接触したときに呼ばれるオーバーラップイベント
     *
     * @param OverlappedComponent オーバーラップされたコンポーネント
     * @param OtherActor 接触した他のアクター
     * @param OtherComponent 接触した他のアクターのコンポーネント
     * @param OtherBodyIndex ボディインデックス
     * @param bFromSweep スイープによる衝突かどうか
     * @param SweepResult 衝突の詳細情報
     */
    UFUNCTION()
    void OverlapDead(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);
};
