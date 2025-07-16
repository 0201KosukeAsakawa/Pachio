// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ColorFilterInterface.h"
#include "Blueprint/UserWidget.h"
#include "ColorLens.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class PACHIO_API UColorLens : public UUserWidget,public IColorReactiveInterface
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void Animation(float DeltaTime); // 使わなければ削除してOK

    // 拍のタイミングで呼ぶ
    UFUNCTION()
    void PlayBeatAnimation();

private:
    void ColorAction(FLinearColor InColor) override;

    UPROPERTY(meta = (BindWidget))
    UImage* FilterColorImage;

    // アニメーション用メンバ変数
    bool bIsAnimating = false;
    float AnimationTime = 0.f;
    float AnimationDuration = 0.3f;   // アニメーション全体時間（秒）
    FVector2D OriginalScale = FVector2D(1.f, 1.f);
    FVector2D TargetScale = FVector2D(1.3f, 1.3f); // 拍で拡大する倍率
};
