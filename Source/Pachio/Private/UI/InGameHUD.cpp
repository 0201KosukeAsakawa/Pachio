// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameHUD.h"
#include "UI/UIManager.h"
#include "UI/LockonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void AInGameHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!UIManager || !UIManager->GetMarker())
        return;

    ULockonWidget* MarkerWidget = UIManager->GetMarker();
    AActor* Target = MarkerWidget->GetTargetActor();
    if (!Target) return;

    FVector2D ScreenPos = FVector2D{0,0};
    APlayerController* PlayerController = GetOwningPlayerController();

    if (PlayerController->ProjectWorldLocationToScreen(Target->GetActorLocation(), ScreenPos))
    {
        float Scale = UWidgetLayoutLibrary::GetViewportScale(this); // this = any widget
        ScreenPos /= Scale;

        if (MarkerWidget)
        {
            MarkerWidget->UpdateScreenPosition(ScreenPos); // false = è“®‚Å•â³Ï‚İ
        }
    }
}