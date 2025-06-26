// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/DeadPlayerState.h"
#include "Interface/StateControllable.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"
#include "Kismet/GameplayStatics.h" // �����ǉ�

bool UDeadPlayerState::OnEnter(ACharacter* Owner, UWorld* World)
{
    if (!Owner || !World)
        return false;

    // UI表示
    if (ALevelManager* LevelManager = ALevelManager::GetInstance(World))
    {
        if (auto UIManager = LevelManager->GetUIManager())
        {
            UIManager->ShowWidget(EWidgetCategory::Tutorial, TEXT("Dead"));
        }
    }

    // カーソルを表示＆入力モード切り替え
    if (APlayerController* PC = Cast<APlayerController>(Owner->GetController()))
    {
        PC->bShowMouseCursor = true;

        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
    }

    // ゲームをポーズする
    if (UGameplayStatics::GetPlayerController(World, 0))
    {
        UGameplayStatics::SetGamePaused(World, true);
    }

    return true;
}
