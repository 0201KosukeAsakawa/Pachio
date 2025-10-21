#include "UI/UIManager.h"
#include "UI/ColorLens.h"
#include "UI/LockonWidget.h"
#include "UI/ClearResultWidget.h"
#include "UI/PlayAnimationWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/Color/ColorControllerComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/InGameHUD.h"

void UUIManager::Init(const AActor*)
{
    // 全カテゴリのウィジェットを初期化
    InitAllWidgets();

    // HUD と接続し、UIManager をセット
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC == nullptr)
        return;

    AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
    if (MyHUD)
    {
        MyHUD->SetUIManager(this);
    }

    // カラーレンズのUIが存在すれば表示
    if (ColorLens)
        ColorLens->AddToViewport();

    // Markerカテゴリのウィジェットを専用マップに登録
    if (FWidgetData* WidgetSet = WidgetDataMap.Find(EWidgetCategory::Marker))
    {
        for (auto& Pair : WidgetSet->WidgetMap)
        {
            if (ULockonWidget* LockonWidget = Cast<ULockonWidget>(Pair.Value))
            {
                MarkerWidgets.Add(Pair.Key, LockonWidget);
            }
        }
    }
}

void UUIManager::InitAllWidgets()
{
    // 各カテゴリに属するウィジェットをまとめて初期化
    for (auto& Pair : WidgetDataMap)
    {
        InitWidgetGroup(Pair.Value);
    }
}

void UUIManager::InitWidgetGroup(FWidgetData& WidgetGroup)
{
    // 既存のインスタンスをクリア
    WidgetGroup.WidgetMap.Reset();

    // クラス定義に基づき、ウィジェットを生成して登録
    for (auto& ClassPair : WidgetGroup.WidgetClassMap)
    {
        if (!ClassPair.Value)
            continue;

        UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), ClassPair.Value);
        if (NewWidget)
        {
            NewWidget->RemoveFromParent(); // 念のため親から外してから登録
            WidgetGroup.WidgetMap.Add(ClassPair.Key, NewWidget);
        }
    }
}

void UUIManager::CreateWidgetArray(const TArray<TSubclassOf<UUserWidget>>& Classes, TArray<UUserWidget*>& Widgets)
{
    // 複数クラスからウィジェットを生成する汎用関数
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

UUserWidget* UUIManager::ShowWidget(EWidgetCategory CategoryName, FName WidgetName)
{
    UUserWidget* widget = nullptr;

    // 該当カテゴリが存在しなければ無視
    if (!WidgetDataMap.Contains(CategoryName))
        return widget;

    FWidgetData& Group = WidgetDataMap[CategoryName];
    UUserWidget** FoundWidget = Group.WidgetMap.Find(WidgetName);
    if (FoundWidget == nullptr)
        return widget;

    // すでに表示中のウィジェットなら再利用
    if (!Group.CurrentWidget.IsEmpty())
    {
        if (Group.CurrentWidget.Contains(WidgetName))
        {
            Group.CurrentWidget[WidgetName]->AddToViewport();
            return Group.CurrentWidget[WidgetName];
        }
    }

    // 新しく表示リストに追加
    Group.CurrentWidget.Add(WidgetName, *FoundWidget);
    Group.CurrentWidget[WidgetName]->AddToViewport();

    widget = Group.CurrentWidget[WidgetName];
    return widget;
}

const void UUIManager::HideCurrentWidget(EWidgetCategory CategoryName, FName WidgetName)
{
    // カテゴリが存在しなければ無視
    if (!WidgetDataMap.Contains(CategoryName))
        return;

    FWidgetData& Group = WidgetDataMap[CategoryName];

    if (!Group.CurrentWidget.Contains(WidgetName))
        return;

    // アニメーション対応ウィジェットなら停止
    if (UUserWidget* widget = Group.CurrentWidget[WidgetName])
    {
        if (UPlayAnimationWidget* animWidget = Cast<UPlayAnimationWidget>(widget))
        {
            animWidget->StopAllAnimation();
        }
    }

    // ビューポートから削除
    RemoveWidgetFromViewport(Group.CurrentWidget[WidgetName]);
}

bool UUIManager::IsWidgetVisible(EWidgetCategory CategoryName, FName WidgetName) const
{
    // 指定カテゴリが存在するかチェック
    const FWidgetData* Group = WidgetDataMap.Find(CategoryName);
    if (!Group)
        return false;
    if (!Group->CurrentWidget[WidgetName])
        return false;

    return true;
}

UUserWidget* UUIManager::GetWidget(EWidgetCategory CategoryName, FName WidgetName)
{
    // 該当カテゴリが存在しなければ無視
    if (!WidgetDataMap.Contains(CategoryName))
        return nullptr;

    FWidgetData& Group = WidgetDataMap[CategoryName];
    UUserWidget** FoundWidget = Group.WidgetMap.Find(WidgetName);
    if (!FoundWidget)
        return nullptr;

    return *FoundWidget;
}

bool UUIManager::PlayWidgetAnimation(EWidgetCategory CategoryName, FName WidgetName, FName AnimationName)
{
    if (!WidgetDataMap.Contains(CategoryName))
        return false;

    FWidgetData& Group = WidgetDataMap[CategoryName];
    UUserWidget** FoundWidgetPtr = Group.CurrentWidget.Find(WidgetName);
    if (!FoundWidgetPtr || !*FoundWidgetPtr)
        return false;

    // アニメーション再生対応のウィジェットにキャストして実行
    UPlayAnimationWidget* animWidget = Cast<UPlayAnimationWidget>(*FoundWidgetPtr);
    if (animWidget == nullptr)
        return false;

    animWidget->PlayAnimationByName(AnimationName);
    return true;
}

void UUIManager::RemoveWidgetFromViewport(UUserWidget*& Widget)
{
    // ウィジェットをビューポートから削除
    if (Widget)
    {
        Widget->RemoveFromViewport();
    }
}

UUserWidget* UUIManager::ShowResultWidget(float Time, EStageRank Rank)
{
    // リザルト画面を表示し、内容をセット
    UUserWidget* BaseWidget = ShowWidget(EWidgetCategory::Menu, "Result");

    if (UClearResultWidget* ResultWidget = Cast<UClearResultWidget>(BaseWidget))
    {
        ResultWidget->SetupClearResult(Time, Rank);
    }

    return BaseWidget;
}

UUserWidget* UUIManager::ShowMarker(FName MarkerName, AActor* Target)
{
    // ターゲットが無効なら無視
    if (!Target)
        return nullptr;

    // マーカー名に対応するウィジェットを取得
    ULockonWidget** FoundWidget = MarkerWidgets.Find(MarkerName);
    if (!FoundWidget || !(*FoundWidget))
        return nullptr;

    ULockonWidget* MarkerWidget = *FoundWidget;

    // 未表示なら表示
    if (!MarkerWidget->IsInViewport())
    {
        MarkerWidget->AddToViewport();
    }

    // マーカーの追従対象をセット
    MarkerWidget->SetTargetActor(Target);

    return MarkerWidget;
}

void UUIManager::HideMarker(FName MarkerName)
{
    // 指定マーカーを非表示
    ULockonWidget** FoundWidget = MarkerWidgets.Find(MarkerName);
    if (FoundWidget && *FoundWidget)
    {
        (*FoundWidget)->RemoveFromParent();
    }
}

const TMap<FName, ULockonWidget*>& UUIManager::GetAllMarkers() const
{
    // 全てのロックオンマーカーを返す
    return MarkerWidgets;
}
