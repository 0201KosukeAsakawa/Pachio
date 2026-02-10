// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIHandle.h"
#include "UI/UIManager.h"
#include "Manager/LevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"
#include "UI/InGameHUD.h"

UUIManager* UUIHandle::GetUIManager(UObject* WorldContext)
{
    if (!WorldContext)
    {
        UE_LOG(LogTemp, Warning, TEXT("UIHandle: WorldContext is null"));
        return nullptr;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("UIHandle: PlayerController not found"));
        return nullptr;
    }

    ALevelManager* LevelManager = ALevelManager::GetInstance(WorldContext);
    if (!LevelManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("UIHandle: InGameHUD not found"));
        return nullptr;
    }

    UUIManager* UIManager = LevelManager->GetUIManager();
    if (!UIManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("UIHandle: UIManager not found"));
        return nullptr;
    }

    return UIManager;
}

// ==========================
// ==== ウィジェット表示系 ===
// ==========================

UUserWidget* UUIHandle::ShowWidget(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->ShowWidget(CategoryName, WidgetName);
    }
    return nullptr;
}

void UUIHandle::HideWidget(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        UIManager->HideCurrentWidget(CategoryName, WidgetName);
    }
}

bool UUIHandle::IsWidgetVisible(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->IsWidgetVisible(CategoryName, WidgetName);
    }
    return false;
}

UUserWidget* UUIHandle::GetWidget(UObject* WorldContext, EWidgetCategory CategoryName, FName WidgetName)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->GetWidget(CategoryName, WidgetName);
    }
    return nullptr;
}

// ==========================
// ==== アニメーション再生 ====
// ==========================

bool UUIHandle::PlayWidgetAnimation(
    UObject* WorldContext,
    EWidgetCategory CategoryName,
    FName WidgetName,
    FName AnimationName,
    bool bReverse,
    float PlaybackSpeed,
    bool bRestoreState
)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->PlayWidgetAnimationWithDirection(
            CategoryName,
            WidgetName,
            AnimationName,
            bReverse,
            PlaybackSpeed,
            bRestoreState
        );
    }
    return false;
}

bool UUIHandle::PlayModeAnimation(UObject* WorldContext, FName WidgetName, FName AnimationName, float PlaybackSpeed)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->PlayModeAnimation(WidgetName,AnimationName, PlaybackSpeed);
    }
    return false;
}

bool UUIHandle::PlayModeAnimationReverse(
    UObject* WorldContext,
    FName WidgetName,
    FName AnimationName,
    float PlaybackSpeed,
    bool bRestoreState
)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->PlayModeAnimationReverse(WidgetName,AnimationName, PlaybackSpeed, bRestoreState);
    }
    return false;
}

// ==========================
// ==== リザルト・マーカー ====
// ==========================

UUserWidget* UUIHandle::ShowResultWidget(UObject* WorldContext, float Time, EStageRank Rank)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->ShowResultWidget(Time, Rank);
    }
    return nullptr;
}

UUserWidget* UUIHandle::ShowMarker(UObject* WorldContext, FName MarkerName, AActor* Target)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        return UIManager->ShowMarker(MarkerName, Target);
    }
    return nullptr;
}

void UUIHandle::HideMarker(UObject* WorldContext, FName MarkerName)
{
    if (UUIManager* UIManager = GetUIManager(WorldContext))
    {
        UIManager->HideMarker(MarkerName);
    }
}