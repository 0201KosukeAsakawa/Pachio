// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/StageInfo.h"
#include "ScoreManager.generated.h"



UCLASS(Blueprintable)
class PACHIO_API UScoreManager : public UObject
{
    GENERATED_BODY()

public:

    /** @brief コンストラクタ。スコアやコインの初期化を行う */
    UScoreManager();

    /**
     * @brief コインを追加する。
     *
     * @param Amount 追加するコイン数。
     */
    void AddCoin(int Amount);

    /**
     * @brief スコアを追加する。
     *
     * @param Amount 追加するスコア値。
     */
    void AddScore(int Amount);

    /** @brief スコア関連の初期化処理を行う */
    void Init();

    /**
     * @brief ゲームクリア時のランクを評価する。
     *
     * @param World 対象のワールドコンテキスト。
     * @return ステージクリアランク（EStageRank）。
     */
    EStageRank EvaluateClearRank(UWorld* World);

    /**
     * @brief ゲーム内経過時間を取得する。
     *
     * @return 経過時間（秒）。
     */
    UFUNCTION(BlueprintCallable, Category = "UIManager")
    inline int GetTime() const { return InGameTimer; }

    /**
     * @brief 現在のゲームスコアを取得する。
     *
     * @return 現在のスコア。
     */
    UFUNCTION(BlueprintCallable)
    inline int GetGameScore() const { return GameScore; }

    /**
     * @brief 現在のコイン取得数を取得する。
     *
     * @return コイン数。
     */
    UFUNCTION(BlueprintCallable)
    inline int GetCoin() const { return Coin; }

    /** @brief 経過時間をカウントアップする */
    void CountDown() { ++InGameTimer; }

private:

    /** @brief 現在のゲームスコア */
    int GameScore;

    /** @brief 取得したコインの合計数 */
    int Coin;

    /** @brief ゲーム内経過時間（秒） */
    UPROPERTY(EditAnywhere)
    float InGameTimer = 0;

    /** @brief Sランク評価用の基準時間 */
    UPROPERTY(EditAnywhere)
    int SRankTime;

    /** @brief Aランク評価用の基準時間 */
    UPROPERTY(EditAnywhere)
    int ARankTime;

    /** @brief タイマー処理用ハンドル */
    FTimerHandle CountTimerHandle;
};
