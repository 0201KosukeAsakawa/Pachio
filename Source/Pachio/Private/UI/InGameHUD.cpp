// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameHUD.h"
#include "UI/UIManager.h"
#include "UI/LockonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void AInGameHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!UIManager)
        return;

    const TMap<FName, ULockonWidget*>& Markers = UIManager->GetAllMarkers();
    APlayerController* PlayerController = GetOwningPlayerController();

    if (!PlayerController)
        return;

    float Scale = UWidgetLayoutLibrary::GetViewportScale(this); // ウィジェットスケール補正

    for (const auto& Pair : Markers)
    {
        ULockonWidget* MarkerWidget = Pair.Value;
        if (!MarkerWidget || !MarkerWidget->IsInViewport())
            continue;

        AActor* Target = MarkerWidget->GetTargetActor();
        if (!Target)
            continue;

        FVector2D ScreenPos;
        if (PlayerController->ProjectWorldLocationToScreen(Target->GetActorLocation(), ScreenPos))
        {
            ScreenPos /= Scale;
            MarkerWidget->UpdateScreenPosition(ScreenPos);
        }
    }
}
