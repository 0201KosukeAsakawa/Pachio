#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataContainer/StageInfo.h"
#include "ClearResultWidget.generated.h"

/**
 * @brief ステージクリア結果を表示するUIウィジェット
 * クリアタイムやステージランクを表示する
 */
UCLASS()
class PACHIO_API UClearResultWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief ウィジェットに値をセットして初期化する
     * @param InClearTime クリア時間（秒）
     * @param InScore ステージランク
     */
    UFUNCTION(BlueprintCallable)
    void SetupClearResult(float InClearTime, EStageRank InScore);

    /**
     * @brief クリア時間をテキスト形式で取得
     * @return 画面に表示するクリア時間のFText
     */
    UFUNCTION(BlueprintPure)
    FText GetClearTimeText() const;

    /**
     * @brief ステージランクをテキスト形式で取得
     * @return 画面に表示するランクのFText
     */
    UFUNCTION(BlueprintPure)
    FText GetRankText() const;

protected:
    /** @brief ウィジェット生成時に呼ばれる初期化処理 */
    virtual void NativeConstruct() override;

    /** @brief クリア結果表示用アニメーションを再生 */
    UFUNCTION()
    void PlayResultAnimation();

    /** @brief アニメーション（Blueprintでバインド） */
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeInAnimation;

private:
    /** キャッシュされたクリア時間 */
    float CachedClearTime = 0.0f;

    /** キャッシュされたステージランク */
    EStageRank CachedRank = EStageRank::B;
};