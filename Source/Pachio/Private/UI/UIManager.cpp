#include "UI/UIManager.h"
#include "UI/ColorLens.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/ColorControllerComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

void UUIManager::Init(const AActor*)
{
    // すべてのカテゴリのウィジェットを初期化
    InitAllWidgets();
}


void UUIManager::InitAllWidgets()
{
    // すべてのカテゴリにあるウィジェットクラスからウィジェットを生成して初期化
    for (auto& Pair : WidgetDataMap)
    {
        InitWidgetGroup(Pair.Value);
    }
}

void UUIManager::InitWidgetGroup(FWidgetData& WidgetGroup)
{
    // ウィジェットインスタンスマップをリセット
    WidgetGroup.WidgetMap.Reset();

    // クラス情報に基づき、各ウィジェットを生成してマップに登録
    for (auto& ClassPair : WidgetGroup.WidgetClassMap)
    {
        if (!ClassPair.Value) 
            continue;

        UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), ClassPair.Value);
        if (NewWidget)
        {
            // 一度親から削除（念のため）
            NewWidget->RemoveFromParent();
            WidgetGroup.WidgetMap.Add(ClassPair.Key, NewWidget);
        }
    }
}

void UUIManager::CreateWidgetArray(const TArray<TSubclassOf<UUserWidget>>& Classes, TArray<UUserWidget*>& Widgets)
{
    // 汎用的なウィジェット初期化
    Widgets.Reset();

    for (auto& WidgetClass : Classes)
    {
        if (!WidgetClass) 
            continue;

        UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (Widget)
        {
            Widget->RemoveFromParent();
            Widgets.Add(Widget);
        }
    }
}

void UUIManager::ShowWidget(EWidgetCategory CategoryName, FName WidgetName)
{
    // 指定カテゴリが存在しない場合は無視
    if (!WidgetDataMap.Contains(CategoryName)) 
        return;

    FWidgetData& Group = WidgetDataMap[CategoryName];

    // 指定名のウィジェットを検索し、ビューポートに表示
    if (UUserWidget** FoundWidget = Group.WidgetMap.Find(WidgetName))
    {
        Group.CurrentWidget.Add(WidgetName, *FoundWidget);
        Group.CurrentWidget[WidgetName]->AddToViewport();
    }
}

void UUIManager::HideCurrentWidget(EWidgetCategory CategoryName, FName WidgetName)
{
    // 指定カテゴリが存在しない場合は無視
    if (!WidgetDataMap.Contains(CategoryName)) 
        return;

    FWidgetData& Group = WidgetDataMap[CategoryName];
    if (!Group.CurrentWidget[WidgetName])
        return;

    // 現在のウィジェットを非表示にして nullptr に
    RemoveWidgetFromViewport(Group.CurrentWidget[WidgetName]);
}

bool UUIManager::IsWidgetVisible(EWidgetCategory CategoryName , FName WidgetName) const
{
    const FWidgetData* Group = WidgetDataMap.Find(CategoryName);

    // 指定カテゴリのウィジェットがビューポート上で可視か判定
    if (!Group)
        return false; 
    if (!Group->CurrentWidget[WidgetName])
        return false;


    return true;
}

UUserWidget* UUIManager::GetWidget(EWidgetCategory CategoryName, FName WidgetName) 
{
    // 指定カテゴリが存在しない場合は無視
    if (!WidgetDataMap.Contains(CategoryName))
        return nullptr;

     FWidgetData& Group = WidgetDataMap[CategoryName];
     UUserWidget** FoundWidget = Group.WidgetMap.Find(WidgetName);
    // 指定名のウィジェットを検索し、ビューポートに表示
    if (!FoundWidget)
        return nullptr;
    
    return *FoundWidget;
}

void UUIManager::RemoveWidgetFromViewport(UUserWidget*& Widget)
{
    // ウィジェットが存在していればビューポートから削除し、ポインタもリセット
    if (Widget)
    {
        Widget->RemoveFromViewport();
        Widget = nullptr;
    }
}