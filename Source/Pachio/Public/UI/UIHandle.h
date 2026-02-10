// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataContainer/UIStruct.h"
#include "DataContainer/StageInfo.h"
#include "UIHandle.generated.h"

class UUIManager;
class UUserWidget;

/**
 * @brief UIManager への静的アクセスを提供するヘルパークラス
 * Blueprint から簡単に UI 操作を行えるようにする
 */
UCLASS()
class PACHIO_API UUIHandle : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

private:
    /**
     * @brief UIManager インスタンスを取得
     * @param WorldContext ワールドコンテキスト
     * @return UUIManager* ポインタ
     */
    static UUIManager* GetUIManager(UObject* WorldContext);

public:
    // ============================
    // ==== ウィジェット表示系 ===
    // ============================

    /**
     * @brief 指定したカテゴリ・名前のウィジェットを表示
     * @param WorldContext ワールドコンテキスト
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @return 表示された UUserWidget* ポインタ
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContext"))
    static UUserWidget* ShowWidget(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName);

    /**
     * @brief 指定カテゴリ・ウィジェットを非表示にする
     * @param WorldContext ワールドコンテキスト
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContext"))
    static void HideWidget(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName);

    /**
     * @brief 指定カテゴリ・ウィジェットが表示中かを取得
     * @param WorldContext ワールドコンテキスト
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @return bool 表示中かどうか
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContext"))
    static bool IsWidgetVisible(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName);

    /**
     * @brief 指定カテゴリ・ウィジェットの取得
     * @param WorldContext ワールドコンテキスト
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @return UUserWidget* ポインタ
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContext"))
    static UUserWidget* GetWidget(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName);

    // ============================
    // ==== アニメーション再生 ====
    // ============================

    /**
     * @brief ウィジェット内アニメーション再生
     * @param WorldContext ワールドコンテキスト
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @param AnimationName 再生するアニメーション名
     * @param bReverse true で逆再生、false で順再生
     * @param PlaybackSpeed 再生速度
     * @param bRestoreState 逆再生終了時に初期状態に戻すか
     * @return bool 成功したか
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContext"))
    static bool PlayWidgetAnimation(
        UObject* WorldContext,
        EWidgetCategory CategoryName,
        FName WidgetName,
        FName AnimationName,
        bool bReverse = false,
        float PlaybackSpeed = 1.0f,
        bool bRestoreState = false
    );

    /**
     * @brief モードウィジェットのアニメーションを順再生
     * @param WorldContext ワールドコンテキスト
     * @param AnimationName 再生するアニメーション名
     * @param PlaybackSpeed 再生速度
     * @return bool 成功したか
     */
    UFUNCTION(BlueprintCallable, Category = "UI|Mode", meta = (WorldContext = "WorldContext"))
    static bool PlayModeAnimation(UObject* WorldContext, FName WidgetName, FName AnimationName, float PlaybackSpeed = 1.0f);

    /**
     * @brief モードウィジェットのアニメーションを逆再生
     * @param WorldContext ワールドコンテキスト
     * @param AnimationName 再生するアニメーション名
     * @param PlaybackSpeed 再生速度
     * @param bRestoreState 逆再生終了時に初期状態に戻すか
     * @return bool 成功したか
     */
    UFUNCTION(BlueprintCallable, Category = "UI|Mode", meta = (WorldContext = "WorldContext"))
    static bool PlayModeAnimationReverse(
        UObject* WorldContext,
        FName WidgetName,
        FName AnimationName,
        float PlaybackSpeed = 1.0f,
        bool bRestoreState = false
    );

    // ============================
    // ==== リザルト・マーカー ====
    // ============================

    /**
     * @brief 結果表示用ウィジェットを表示
     * @param WorldContext ワールドコンテキスト
     * @param Time 表示時間
     * @param Rank ステージ評価ランク
     * @return UUserWidget* ポインタ
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContext"))
    static UUserWidget* ShowResultWidget(UObject* WorldContext, float Time, EStageRank Rank);

    /**
     * @brief 特定対象を追従するマーカーを表示
     * @param WorldContext ワールドコンテキスト
     * @param MarkerName マーカー名
     * @param Target 追従対象のアクター
     * @return UUserWidget* ポインタ
     */
    UFUNCTION(BlueprintCallable, Category = "UI|Marker", meta = (WorldContext = "WorldContext"))
    static UUserWidget* ShowMarker(UObject* WorldContext, FName MarkerName, AActor* Target);

    /**
     * @brief 指定マーカーを非表示にする
     * @param WorldContext ワールドコンテキスト
     * @param MarkerName マーカー名
     */
    UFUNCTION(BlueprintCallable, Category = "UI|Marker", meta = (WorldContext = "WorldContext"))
    static void HideMarker(UObject* WorldContext, FName MarkerName);
};