// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ColorFilterInterface.h"
#include "Blueprint/UserWidget.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorLens.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class PACHIO_API UColorLens : public UUserWidget, public IColorReactiveInterface
{
    GENERATED_BODY()

public:
    /**
     * ウィジェット生成時の初期化処理を行う
     */
    virtual void NativeConstruct() override;

    /**
     * 毎フレーム呼ばれる更新処理
     *
     * @param MyGeometry ジオメトリ情報
     * @param InDeltaTime 前フレームからの経過時間
     */
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /**
     * 拡大縮小アニメーションを行う
     *
     * @param DeltaTime 前フレームからの経過時間
     * 
     *  使わなければ削除してOK
     */
    void Animation(float DeltaTime);

    /**
     * 拍のタイミングで呼ばれ、アニメーションを再生する
     */
    UFUNCTION()
    void PlayBeatAnimation();

private:
    /**
     * 色一致時に呼ばれ、フィルター色を適用する
     *
     * @param NewColor 適用する新しい色
     */
    void ApplyColorWithMatching(const FLinearColor& NewColor) override;

    /** 現在のフィルター色を表示するイメージ */
    UPROPERTY(meta = (BindWidget))
    UImage* FilterColorImage;

    /** 色の円を表すイメージ */
    UPROPERTY(meta = (BindWidget))
    UImage* ColorCircle;

    /** アニメーションが再生中かを示すフラグ */
    bool bIsAnimating = false;

    /** アニメーション経過時間 */
    float AnimationTime = 0.f;

    /** アニメーションの総時間（秒） */
    float AnimationDuration = 0.3f;

    /** 元のスケール */
    FVector2D OriginalScale = FVector2D(1.f, 1.f);

    /** 拍に合わせて拡大する際のスケール倍率 */
    FVector2D TargetScale = FVector2D(1.3f, 1.3f);
};
