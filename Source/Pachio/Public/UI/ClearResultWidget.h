#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataContainer/ClearScoreType.h"
#include "ClearResultWidget.generated.h"

UCLASS()
class PACHIO_API UClearResultWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 値をセットしてウィジェットを初期化
    UFUNCTION(BlueprintCallable)
    void SetupClearResult(float InClearTime, EClearScore InScore);

    // UIバインド用
    UFUNCTION(BlueprintPure)
    FText GetClearTimeText() const;

    UFUNCTION(BlueprintPure)
    FText GetRankText() const;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void PlayResultAnimation();

    // アニメーション（Blueprintでバインドする）
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeInAnimation;

private:
    float CachedClearTime = 0.0f;
   EClearScore CachedRank = EClearScore::B;
};