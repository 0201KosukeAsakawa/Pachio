#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Containers/Map.h"
#include "DataContainer/UIStruct.h"
#include "DataContainer/StageInfo.h"
#include "UIManager.generated.h"

class UWidgetComponent;
class ULockonWidget;
class UColorLens;


 /**
  * @brief ゲーム全体で UI を一元管理するクラス
  * HUD に依存せず、UObjectベースでウィジェット管理を行う
  */
UCLASS(Blueprintable)
class PACHIO_API UUIManager : public UObject
{
    GENERATED_BODY()

public:
    /**
     * @brief UIManagerの初期化
     * @param InActor 参照するアクター（通常はプレイヤーやHUD）
     */
    virtual void Init(const AActor* InActor);

public:
    // ============================
    // ==== ウィジェット表示系 ===
    // ============================

    /**
     * @brief 指定したカテゴリ・名前のウィジェットを表示
     * @param CategoryName ウィジェットカテゴリ（State, Combat, Inventory など）
     * @param WidgetName ウィジェット名
     * @return 表示された UUserWidget* ポインタ
     */
    UFUNCTION(BlueprintCallable)
    UUserWidget* ShowWidget(EWidgetCategory CategoryName, FName WidgetName);

    /**
     * @brief 指定カテゴリ・ウィジェットを非表示にする
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     */
    UFUNCTION(BlueprintCallable)
    const void HideCurrentWidget(EWidgetCategory CategoryName, FName WidgetName);

    /**
     * @brief 指定カテゴリ・ウィジェットが表示中かを取得
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @return bool 表示中かどうか
     */
    UFUNCTION(BlueprintCallable)
    bool IsWidgetVisible(EWidgetCategory CategoryName, FName WidgetName) const;

    /**
     * @brief 指定カテゴリ・ウィジェットの取得
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @return UUserWidget* ポインタ
     */
    UFUNCTION(BlueprintCallable)
    UUserWidget* GetWidget(EWidgetCategory CategoryName, FName WidgetName);

    /**
     * @brief ウィジェット内アニメーション再生
     * @param CategoryName ウィジェットカテゴリ
     * @param WidgetName ウィジェット名
     * @param AnimationName 再生するアニメーション名
     * @return bool 成功したか
     */
    UFUNCTION()
    bool PlayWidgetAnimation(EWidgetCategory CategoryName, FName WidgetName, FName AnimationName);

    /** @brief カラー関連エフェクトを取得 */
    UFUNCTION()
    UColorLens* GetColorLens() { return ColorLens; }

    /**
     * @brief 結果表示用ウィジェットを表示
     * @param Time 表示時間
     * @param Rank ステージ評価ランク
     * @return UUserWidget* ポインタ
     */
    UUserWidget* ShowResultWidget(float Time, EStageRank Rank);

    /**
     * @brief 特定対象を追従するマーカーを表示
     * @param MarkerName マーカー名
     * @param Target 追従対象のアクター
     * @return UUserWidget* ポインタ
     */
    UUserWidget* ShowMarker(FName MarkerName, AActor* Target);

    /**
     * @brief 指定マーカーを非表示にする
     * @param MarkerName マーカー名
     */
    void HideMarker(FName MarkerName);

    /**
     * @brief 登録済み全マーカー取得
     * @return TMap<FName, ULockonWidget*> マーカー名とウィジェットマップ
     */
    const TMap<FName, ULockonWidget*>& GetAllMarkers() const;

    /**
     * @brief 指定マーカー取得
     * @param MarkerName マーカー名
     * @return ULockonWidget* ポインタ
     */
    ULockonWidget* GetMarker(FName MarkerName) const { return MarkerWidgets[MarkerName]; }

private:
    // ============================
    // ==== ウィジェット初期化系 ==
    // ============================

    /** @brief 全カテゴリのウィジェット初期化 */
    void InitAllWidgets();

    /**
     * @brief 指定カテゴリ内のウィジェットを生成しマップに登録
     * @param WidgetGroup 初期化対象のウィジェットデータ
     */
    void InitWidgetGroup(FWidgetData& WidgetGroup);

    /**
     * @brief 配列形式ウィジェットの生成（CrossHair等旧仕様対応）
     * @param WidgetClasses 生成するウィジェットクラス配列
     * @param OutWidgets 生成結果を格納する配列
     */
    void CreateWidgetArray(const TArray<TSubclassOf<UUserWidget>>& WidgetClasses, TArray<UUserWidget*>& OutWidgets);

    /**
     * @brief ウィジェットをビューポートから削除し nullptr にする
     * @param Widget 削除対象のウィジェット
     */
    void RemoveWidgetFromViewport(UUserWidget*& Widget);

private:
    // ============================
    // ==== 変数 ==================
    // ============================

    /** @brief ウィジェットカテゴリごとのデータマップ */
    UPROPERTY(EditAnywhere, Category = "UI")
    TMap<EWidgetCategory, FWidgetData> WidgetDataMap;

    /** @brief 色関連エフェクト管理 */
    UPROPERTY(EditAnywhere)
    UColorLens* ColorLens;

    /** @brief 登録済みマーカーウィジェット */
    TMap<FName, ULockonWidget*> MarkerWidgets;
};
