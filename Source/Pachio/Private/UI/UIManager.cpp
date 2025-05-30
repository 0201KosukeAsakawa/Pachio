#include "UI/UIManager.h"
#include "UI/ColorLens.h"
#include "Components/ColorLensComponent.h"
#include "Blueprint/UserWidget.h"

void UUIManager::Init()
{
    // ウィジェットの初期化（すべてのカテゴリに対して）
    InitAllWidgets();
    BindColorLensComponent();
    ShowWidget(EWidgetCategory::Lens, "ColorLensWidget");
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

    //// 現在表示中のウィジェットがあれば削除
    //RemoveWidgetFromViewport(Group.CurrentWidget);

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

void UUIManager::RemoveWidgetFromViewport(UUserWidget*& Widget)
{
    // ウィジェットが存在していればビューポートから削除し、ポインタもリセット
    if (Widget)
    {
        Widget->RemoveFromViewport();
        Widget = nullptr;
    }
}

void UUIManager::BindColorLensComponent()
{
    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn) return;

    UColorLensComponent* LensComp = PlayerPawn->FindComponentByClass<UColorLensComponent>();
    if (!LensComp) return;

    // ウィジェットの取得
    FWidgetData* WidgetGroup = WidgetDataMap.Find(EWidgetCategory::Lens);
    if (!WidgetGroup) return;

    UUserWidget** WidgetPtr = WidgetGroup->WidgetMap.Find(TEXT("ColorLensWidget"));
    if (!WidgetPtr) return;

    UColorLens* ColorLensWidget = Cast<UColorLens>(*WidgetPtr);
    if (!ColorLensWidget) return;

    // デリゲートバインド
    LensComp->OnColorChanged.AddDynamic(ColorLensWidget, &UColorLens::UpdateFilterColor);

    // 初期色の反映も忘れずに
    ColorLensWidget->UpdateFilterColor(LensComp->GetCurrentColor());
}
