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
 * ゲーム全体で UI を一元管理する HUD 派生クラス
 */
UCLASS(Blueprintable)
class PACHIO_API UUIManager : public UObject
{
    GENERATED_BODY()

public:
    virtual void Init(const AActor*);

public:
    /** 指定したカテゴリと名前のウィジェットを表示する */
    UFUNCTION(BlueprintCallable)
    UUserWidget* ShowWidget(EWidgetCategory CategoryName, FName WidgetName);

    /** 指定カテゴリの現在のウィジェットを非表示にする */
    UFUNCTION(BlueprintCallable)
    void HideCurrentWidget(EWidgetCategory CategoryName, FName WidgetName);

    /** 指定カテゴリの現在のウィジェットが表示中かを取得 */
    UFUNCTION(BlueprintCallable)
    bool IsWidgetVisible(EWidgetCategory CategoryName, FName WidgetName) const;

    UFUNCTION(BlueprintCallable)
    UUserWidget* GetWidget(EWidgetCategory CategoryName, FName WidgetName);

    UFUNCTION()
    UColorLens* GetColorLens() { return ColorLens; }    
    
    UUserWidget* ShowResultWidget(float Time, EStageRank Rank);

    UUserWidget* ShowMarker(FName MarkerName, AActor* Target);

    void HideMarker(FName MarkerName);

    const TMap<FName, ULockonWidget*>& GetAllMarkers() const;

    ULockonWidget* GetMarker(FName MarkerName) const { return MarkerWidgets[MarkerName]; }


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
    TMap<EWidgetCategory, FWidgetData> WidgetDataMap;

    UPROPERTY(EditAnywhere)
    UColorLens* ColorLens;

private:
    TMap<FName, ULockonWidget*> MarkerWidgets;
};