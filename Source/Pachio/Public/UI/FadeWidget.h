// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/Delegate.h"
#include "FadeWidget.generated.h"


/**
 * @brief 画面フェードを制御するUIウィジェット
 * フェードイン・フェードアウトをBlueprintおよびC++から制御可能
 */
UCLASS()
class PACHIO_API UFadeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief フェード完了時に呼ばれるデリゲート
     */
    FSimpleDelegate OnFadeFinished;

    /**
     * @brief Blueprint側でフェード再生を実装するためのイベント
     * @param Duration フェードにかける時間（秒）
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Fade")
    void PlayFadeBP(float Duration);

    /**
     * @brief C++側からフェード再生を呼び出すラッパー
     * @param Duration フェードにかける時間（秒）
     * @param Delegate フェード完了時に呼び出されるコールバック
     */
    void PlayFade(float Duration, FSimpleDelegate Delegate);

    /**
     * @brief フェード完了時に呼ばれる関数
     * Blueprint側のアニメーション終了通知から呼び出される
     */
    UFUNCTION(BlueprintCallable, Category = "Fade")
    void FadeFinished();
};
