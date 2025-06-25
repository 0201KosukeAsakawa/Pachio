// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StageSelectWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"
#include "UI/StageCardWidget.h"

void UStageSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // //仮ステージを作成（あとでDataTableやSaveGameと連携してOK）
    //StageList = {
    //    {TEXT("Stage01"), TEXT("STAGE 1"), 1, EStageRank::A, nullptr, true},
    //    {TEXT("Stage02"), TEXT("STAGE 2"), 2, EStageRank::None, nullptr, true},
    //    {TEXT("Stage03"), TEXT("STAGE 3"), 3, EStageRank::S, nullptr, true}
    //};

    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &UStageSelectWidget::OnPlayPressed);

   UpdateCards();

    // 入力バインド（例：左右キー）
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->InputComponent->BindAction("UI_Left", IE_Pressed, this, &UStageSelectWidget::MoveSelectionLeft);
        PC->InputComponent->BindAction("UI_Right", IE_Pressed, this, &UStageSelectWidget::MoveSelectionRight);
    }
}

void UStageSelectWidget::MoveSelection(int32 Offset)
{
    CurrentIndex = FMath::Clamp(CurrentIndex + Offset, 0, StageList.Num() - 1);
    UpdateCards();
}

void UStageSelectWidget::UpdateCards()
{
    if (!CardBox || !StageCardClass) return;

    CardBox->ClearChildren();
    StageWidgetNames.Empty();

    ALevelManager* AL = ALevelManager::GetInstance(GetWorld());
    if (!AL) return;

    UUIManager* UM = AL->GetUIManager();
    if (!UM) return;

    int32 i = CurrentIndex;  // 今回は1枚だけ

    if (i >= 0 && i < StageList.Num())
    {
        FName WidgetName = FName(*FString::Printf(TEXT("StageCard_%d"), i));
        StageWidgetNames.Add(WidgetName);

        UUserWidget* WidgetBase = UM->ShowWidget(EWidgetCategory::Menu, WidgetName);
        UStageCardWidget* CardWidget = Cast<UStageCardWidget>(WidgetBase);

        if (CardWidget)
        {
            if (CardWidget->IsInViewport())
            {
                CardWidget->RemoveFromParent();
            }

            CardWidget->SetStageInfo(StageList[i], true);
            CardBox->AddChildToHorizontalBox(CardWidget);
        }
    }
}


void UStageSelectWidget::OnPlayPressed()
{
    const FStageInfo& Info = StageList[CurrentIndex];
    if (Info.bUnlocked)
    {
        UGameplayStatics::OpenLevel(this, FName(*Info.StageID));
    }
}


void UStageSelectWidget::MoveSelectionLeft()
{
    MoveSelection(-1);
}

void UStageSelectWidget::MoveSelectionRight()
{
    MoveSelection(1);
}