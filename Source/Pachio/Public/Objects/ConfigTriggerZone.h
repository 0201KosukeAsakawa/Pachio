// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CameraHandlerComponent.h"
#include "ConfigTriggerZone.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API AConfigTriggerZone : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。トリガーゾーンの初期化を行う。
     */
    AConfigTriggerZone();

protected:
    /**
     * @brief ゲーム開始時またはスポーン時に呼ばれる初期化処理
     */
    virtual void BeginPlay() override;

private:
    /**
     * @brief 当たり判定が開始された際に呼ばれるオーバーラップイベント
     *
     * @param OverlappedComp オーバーラップされたコンポーネント
     * @param OtherActor 重なった他のアクター
     * @param OtherComp 他のアクターのコンポーネント
     * @param OtherBodyIndex ボディインデックス
     * @param bFromSweep スイープによる衝突かどうか
     * @param SweepResult 衝突の詳細情報
     */
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

    /** @brief 当たり判定用のボックスコンポーネント */
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* CollisionBox;

    /** @brief このゾーンに適用するカメラビュータイプ */
    UPROPERTY(EditAnywhere, Category = "Grid")
    ECameraViewType CameraViewType = ECameraViewType::GridView;

    /** @brief グリッドサイズ（X, Y） */
    UPROPERTY(EditAnywhere, Category = "Grid")
    FVector2D GridSize;

    /** @brief Z方向のオフセット（高さバッファ） */
    UPROPERTY(EditAnywhere, Category = "Grid")
    float ZBaffer = 9000;
};
