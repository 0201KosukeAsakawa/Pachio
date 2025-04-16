#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Containers/Map.h"
#include "UIManager.generated.h"

/**
 * 複数ウィジェットカテゴリ（例: State, Menu, HUD）に対応するデータ構造
 */
USTRUCT(BlueprintType)
struct FWidgetData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    // ウィジェット名に対応するウィジェットクラス（Blueprint クラス）
    UPROPERTY(EditAnywhere, Category = "UI")
    TMap<FName, TSubclassOf<UUserWidget>> WidgetClassMap;

    // 実行時に生成されたウィジェットのインスタンスを保持
    UPROPERTY(Transient)
    TMap<FName, UUserWidget*> WidgetMap;

    // 現在表示中のウィジェット
    UPROPERTY(Transient)
    UUserWidget* CurrentWidget = nullptr;
};

/**
 * ゲーム全体で UI を一元管理する HUD 派生クラス
 */
UCLASS()
class PACHIO_API AUIManager : public AHUD
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    /** 指定したカテゴリと名前のウィジェットを表示する */
    UFUNCTION(BlueprintCallable)
    void ShowWidget(FName CategoryName, FName WidgetName);

    /** 指定カテゴリの現在のウィジェットを非表示にする */
    UFUNCTION(BlueprintCallable)
    void HideCurrentWidget(FName CategoryName);

    /** 指定カテゴリのウィジェットの表示／非表示を切り替える */
    UFUNCTION(BlueprintCallable)
    void ToggleWidgetVisibility(FName CategoryName, bool bVisible);

    /** 指定カテゴリの現在のウィジェットが表示中かを取得 */
    UFUNCTION(BlueprintCallable)
    bool IsWidgetVisible(FName CategoryName) const;

private:
    /** 全てのカテゴリに対してウィジェットを初期化 */
    void InitAllWidgets();

    /** 指定されたカテゴリ内のウィジェットを生成しマップに追加 */
    void InitWidgetGroup(FWidgetData& WidgetGroup);

    /** 一般配列形式のウィジェット群を初期化（旧 CrossHair 系の用途など） */
    void CreateWidgetArray(const TArray<TSubclassOf<UUserWidget>>&, TArray<UUserWidget*>&);

    /** 指定したウィジェットをビューポートから削除し nullptr にする */
    void RemoveWidgetFromViewport(UUserWidget*& Widget);

private:
    /** 複数のウィジェットカテゴリごとのデータ（State, Combat, Inventory など） */
    UPROPERTY(EditAnywhere, Category = "UI")
    TMap<FName, FWidgetData> WidgetDataMap;

    /** 状態カテゴリの UI が現在表示中かどうかを記録 */
    bool bIsStateWidgetVisible = true;
};