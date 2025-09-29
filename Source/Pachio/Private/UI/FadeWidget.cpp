// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FadeWidget.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"

void UFadeWidget::PlayFade(float Duration, FSimpleDelegate Delegate)
{
    OnFadeFinished = Delegate;
    PlayFadeBP(Duration);
}

void UFadeWidget::FadeFinished()
{
    ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
    if (levelManager == nullptr)
        return;
    UUIManager* uiManager = ALevelManager::GetInstance(GetWorld())->GetUIManager();
    if (uiManager == nullptr)
        return;

    uiManager->HideCurrentWidget(EWidgetCategory::Menu, "Fade");

    OnFadeFinished.ExecuteIfBound();
}