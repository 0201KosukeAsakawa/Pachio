// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LockonWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

void ULockonWidget::UpdateScreenPosition(const FVector2D& ScreenPosition)
{
    if (LockonImage)
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(LockonImage->Slot))
        {
            CanvasSlot->SetPosition(ScreenPosition+ AddVectorScale);
        }
    }
}
void ULockonWidget::SetMarkerVisible(bool bVisible)
{
    if (LockonImage)
        LockonImage->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}