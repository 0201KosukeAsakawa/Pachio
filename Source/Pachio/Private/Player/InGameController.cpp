// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InGameController.h"
#include "Manager/LevelManager.h"
#include "Sound/SoundManager.h"


AInGameController::AInGameController()
    :DetectionDistance(300)
    , BoxHalfSize(FVector(5000.f, 5000.f, 5000.f))
{
}

// 入力をセットアップする関数
void AInGameController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

// 所有権を切り替える関数
void AInGameController::TogglePossession(AActor* HitActor)
{
    // すでに何かを憑依していて、元のPawnが記録されている場合は元に戻る
    if (bIsPossessing && OriginalPawn)
    {
        Possess(OriginalPawn);        // 元のPawnに所有権を戻す
        bIsPossessing = false;        // 所有状態フラグをリセット
        ALevelManager::GetInstance(GetWorld())->GetSoundManager()->PlaySound("SE","PlayChange");
        return;
    }
}

// 手動で元のPawnに戻る処理（外部から呼ばれる用）
void AInGameController::ReturnToOriginalPlayer()
{
    if (OriginalPawn)
    {
        Possess(OriginalPawn); // 元のPawnに所有権を戻す
    }
}