#include "UI/UIManager.h"
#include "Blueprint/UserWidget.h"

void AUIManager::BeginPlay()
{
    Super::BeginPlay();

    // ウィジェットの初期化（すべてのカテゴリに対して）
    InitAllWidgets();
}

void AUIManager::InitAllWidgets()
{
    // すべてのカテゴリにあるウィジェットクラスからウィジェットを生成して初期化
    for (auto& Pair : WidgetDataMap)
    {
        InitWidgetGroup(Pair.Value);
    }
}

void AUIManager::InitWidgetGroup(FWidgetData& WidgetGroup)
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

void AUIManager::CreateWidgetArray(const TArray<TSubclassOf<UUserWidget>>& Classes, TArray<UUserWidget*>& Widgets)
{
    // 汎用的なウィジェット初期化（配列ベース、例：クロスヘアなど）
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

void AUIManager::ShowWidget(FName CategoryName, FName WidgetName)
{
    // 指定カテゴリが存在しない場合は無視
    if (!WidgetDataMap.Contains(CategoryName)) return;

    FWidgetData& Group = WidgetDataMap[CategoryName];

    // 現在表示中のウィジェットがあれば削除
    RemoveWidgetFromViewport(Group.CurrentWidget);

    // 指定名のウィジェットを検索し、ビューポートに表示
    if (UUserWidget** FoundWidget = Group.WidgetMap.Find(WidgetName))
    {
        Group.CurrentWidget = *FoundWidget;
        Group.CurrentWidget->AddToViewport();
    }
}

void AUIManager::HideCurrentWidget(FName CategoryName)
{
    // 指定カテゴリが存在しない場合は無視
    if (!WidgetDataMap.Contains(CategoryName)) 
        return;

    FWidgetData& Group = WidgetDataMap[CategoryName];

    // 現在のウィジェットを非表示にして nullptr に
    RemoveWidgetFromViewport(Group.CurrentWidget);
}

void AUIManager::ToggleWidgetVisibility(FName CategoryName, bool bVisible)
{
    // 指定カテゴリが存在しない場合は無視
    if (!WidgetDataMap.Contains(CategoryName)) 
        return;

    FWidgetData& Group = WidgetDataMap[CategoryName];
    if (!Group.CurrentWidget) 
        return;

    // 表示状態を切り替える
    if (bVisible)
    {
        if (!Group.CurrentWidget->IsInViewport())
        {
            Group.CurrentWidget->AddToViewport();
        }
    }
    else
    {
        if (Group.CurrentWidget->IsInViewport())
        {
            Group.CurrentWidget->RemoveFromViewport();
        }
    }
}

bool AUIManager::IsWidgetVisible(FName CategoryName) const
{
    // 指定カテゴリのウィジェットがビューポート上で可視か判定
    if (const FWidgetData* Group = WidgetDataMap.Find(CategoryName))
    {
        return Group->CurrentWidget && Group->CurrentWidget->IsVisible();
    }
    return false;
}

void AUIManager::RemoveWidgetFromViewport(UUserWidget*& Widget)
{
    // ウィジェットが存在していればビューポートから削除し、ポインタもリセット
    if (Widget)
    {
        Widget->RemoveFromViewport();
        Widget = nullptr;
    }
}