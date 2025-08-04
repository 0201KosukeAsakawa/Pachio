// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StageSelectWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/LevelManager.h"
#include "Manager/SaveManager.h"
#include "UI/UIManager.h"
#include "UI/StageCardWidget.h"

// ウィジェットの初期化処理
void UStageSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 再生ボタンにクリックイベントをバインド
    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &UStageSelectWidget::OnPlayPressed);

    // セーブデータからステージカードを生成
    GenerateStageCardsFromSave();

    // 入力（← →）をバインド
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (PC->InputComponent)
        {
            PC->InputComponent->BindAction("UI_Left", IE_Pressed, this, &UStageSelectWidget::MoveSelectionLeft);
            PC->InputComponent->BindAction("UI_Right", IE_Pressed, this, &UStageSelectWidget::MoveSelectionRight);
        }
    }
}

// セーブデータからステージ情報を読み込み、カードウィジェットを生成
void UStageSelectWidget::GenerateStageCardsFromSave()
{
    if (!CardCanvas || !StageCardClass) return;

    // 既存のカードをクリア
    CardCanvas->ClearChildren();
    StageWidgets.Empty();
    StageList.Empty();

    // JSONからセーブデータを読み込む
    FStageSaveData SavedData = USaveManager::LoadFromJson();

    int32 Index = 0;
    for (const TPair<FString, FSaveData>& Pair : SavedData.Stages)
    {
        // 新しいカードウィジェットを生成
        UStageCardWidget* CardWidget = CreateWidget<UStageCardWidget>(GetWorld(), StageCardClass);
        if (!CardWidget) continue;

        // セーブ情報からステージ情報を構築
        FStageInfo StageInfo;
        StageInfo.StageID = Pair.Key;
        StageInfo.bUnlocked = Pair.Value.bCleared;
        StageInfo.ClearRank = Pair.Value.ClearRank;
        StageInfo.Title = Pair.Value.Title;

        // 初期状態では選択されていないとして設定
        CardWidget->SetStageInfo(StageInfo, false);

        // カードをキャンバスに追加
        CardCanvas->AddChild(CardWidget);

        // スロット設定（ZOrderだけ。位置は後で調整）
        if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(CardWidget->Slot))
        {
            PanelSlot->SetZOrder(Index);
        }

        // 配列にカード情報を追加
        StageWidgets.Add(CardWidget);
        StageList.Add(StageInfo);
        Index++;
    }

    // 最初に選択するインデックスを 0 に
    CurrentIndex = 0;

    // カードの表示位置を更新
    UpdateCardPositions();

    UE_LOG(LogTemp, Log, TEXT("ステージカードを %d 枚生成しました。"), StageList.Num());
}

// ステージカードの位置、サイズ、透明度などを更新して表示を調整
void UStageSelectWidget::UpdateCardPositions()
{
    if (StageWidgets.Num() == 0 || !CardCanvas) return;

    // キャンバスのサイズから画面中央を計算
    const FVector2D CanvasSize = CardCanvas->GetCachedGeometry().GetLocalSize();
    FVector2D CenterPos = CanvasSize;
    CenterPos = FVector2D(100.f, 100.f); // 仮に中央とする


    // 中央と左右カードのスケール・透明度設定
    const float CenterScale = 0.8f;
    const float SideScale = 0.6f;
    const float SideOpacity = 0.5f;
    const float SideOffsetX = 300.f;

    for (int32 i = 0; i < StageWidgets.Num(); ++i)
    {
        UStageCardWidget* Card = StageWidgets[i];
        if (!Card) continue;

        UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(Card->Slot);
        if (!PanelSlot) continue;

        // 現在の選択インデックスとの差分で配置を決定
        int32 OffsetFromCurrent = i - CurrentIndex;

        // 選択中かどうかを渡す
        bool bIsSelected = (OffsetFromCurrent == 0);
        Card->SetStageInfo(StageList[i], bIsSelected);

        if (OffsetFromCurrent == 0)
        {
            // 中央カード
            PanelSlot->SetPosition(CenterPos);
            PanelSlot->SetZOrder(10);
            Card->SetRenderScale(FVector2D(CenterScale, CenterScale));
            Card->SetRenderOpacity(1.0f);
        }
        else if (OffsetFromCurrent == -1)
        {
            // 左側カード
            PanelSlot->SetPosition(CenterPos - FVector2D(SideOffsetX, 0));
            PanelSlot->SetZOrder(5);
            Card->SetRenderScale(FVector2D(SideScale, SideScale));
            Card->SetRenderOpacity(SideOpacity);
        }
        else if (OffsetFromCurrent == 1)
        {
            // 右側カード
            PanelSlot->SetPosition(CenterPos + FVector2D(SideOffsetX, 0));
            PanelSlot->SetZOrder(5);
            Card->SetRenderScale(FVector2D(SideScale, SideScale));
            Card->SetRenderOpacity(SideOpacity);
        }
        else
        {
            // それ以外は画面外に飛ばして非表示扱い
            PanelSlot->SetPosition(FVector2D(-1000.f, -1000.f));
            Card->SetRenderOpacity(0.f);
        }
    }
}

// ステージ選択インデックスを移動（左: -1、右: +1）
void UStageSelectWidget::MoveSelection(int32 Offset)
{
    if (StageList.Num() == 0) return;

    // インデックスを安全に範囲内に収める
    CurrentIndex = FMath::Clamp(CurrentIndex + Offset, 0, StageList.Num() - 1);

    // 表示位置を更新
    UpdateCardPositions();
}

// 左キー入力時の処理
void UStageSelectWidget::MoveSelectionLeft()
{
    MoveSelection(-1);
}

// 右キー入力時の処理
void UStageSelectWidget::MoveSelectionRight()
{
    MoveSelection(1);
}

// 再生ボタンが押されたときの処理
void UStageSelectWidget::OnPlayPressed()
{
    if (StageList.IsValidIndex(CurrentIndex))
    {
        const FStageInfo& Info = StageList[CurrentIndex];

        // アンロックされているステージなら遷移
        if (Info.bUnlocked)
        {
            UGameplayStatics::OpenLevel(this, FName(*Info.StageID));
        }
    }
}
