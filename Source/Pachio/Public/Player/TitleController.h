// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitleController.generated.h"

UENUM(BlueprintType)
/**
 * @brief タイトル画面の状態を定義
 */
    enum class ETitleState : uint8
{
    Title,          ///< タイトル表示中
    FadingToMovie,  ///< タイトルからムービーにフェード中
    InMovie,        ///< ムービー再生中
    FadingToTitle   ///< ムービーからタイトルにフェード中
};

UCLASS()
/**
 * @brief タイトル画面を制御するPlayerControllerクラス
 * ムービー再生、フェード表示、入力判定などを担当
 */
    class PACHIO_API ATitleController : public APlayerController
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ
     * 初期値設定
     */
    ATitleController();

protected:
    /**
     * @brief 入力コンポーネント設定
     * キーや軸入力のバインドをここで行う
     */
    virtual void SetupInputComponent() override;

    /**
     * @brief 軸入力が発生した際に呼ばれる
     * @param Value 入力値
     */
    void OnAnyAxisInput(float Value);

    /**
     * @brief 毎フレーム更新処理
     * @param DeltaSeconds 前フレームからの経過時間
     */
    virtual void Tick(float DeltaSeconds) override;

    /** @brief 任意の入力があった場合に呼ばれる */
    void OnAnyInput();

    /** @brief ムービー再生開始 */
    void StartMovie();

    /** @brief ムービー再生終了 */
    void EndMovie();

private:
    /**
     * @brief フェード処理を表示
     * @param Duration フェードにかける時間（秒）
     * @param OnFinished フェード完了時に呼ばれるデリゲート
     */
    void ShowFade(float Duration, FSimpleDelegate OnFinished);

    /** @brief ムービーWidgetを表示 */
    void ShowMovie();

    /** @brief ムービーWidgetを非表示 */
    void HideMovie();

private:
    /** アイドル状態のカウンター */
    float IdleCounter;

    /** アイドル判定までの閾値 */
    float IdleThreshold;

    /** 現在のタイトル状態 */
    ETitleState CurrentState;

    // ============================
    // ==== ウィジェット関連 ======
    // ============================

    /** フェード用Widgetのクラス（Blueprint設定用） */
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> FadeWidgetClass;

    /** 実際に使用するフェードWidget */
    UPROPERTY()
    class UFadeWidget* FadeWidget;

    /** ムービー用Widgetのクラス（Blueprint設定用） */
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> MovieWidgetClass;

    /** 実際に使用するムービーWidget */
    UPROPERTY()
    class UMovieWidget* MovieWidget;
};
