// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "LadderActor.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API ALadderActor : public AColorReactiveObject
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。ラダーアクターの初期化を行う。
     */
    ALadderActor();

public:
    /**
     * @brief 毎フレーム呼び出される更新処理
     *
     * @param DeltaTime 前フレームからの経過時間
     */
    virtual void Tick(float DeltaTime) override;

    /**
     * @brief ラダーの上端位置をワールド座標で取得する
     *
     * @return 上端のワールド座標
     */
    FVector GetTopWorldPosition() const;

    /**
     * @brief ラダーの下端位置をワールド座標で取得する
     *
     * @return 下端のワールド座標
     */
    FVector GetBottomWorldPosition() const;

    /**
     * @brief 指定アクターが登るための固定位置を取得する
     *
     * @param OtherActor 対象アクター
     * @return アクターがラダーに沿って移動する際の固定座標
     */
    FVector GetFixedPositionForActor(AActor* OtherActor) const;

    /**
     * @brief 指定アクターがラダーを向くための回転を取得する
     *
     * @param OtherActor 対象アクター
     * @return アクターがラダー方向を向く回転値
     */
    FRotator GetRotationForActorToFaceThis(AActor* OtherActor) const;


private:

    /** @brief ラダーの上下端からのオフセット値 */
    UPROPERTY(EditAnywhere)
    float Offset;

    /** @brief ラダー判定用のボリュームコンポーネント */
    UPROPERTY(EditAnywhere)
    UBoxComponent* LadderVolume;
};
