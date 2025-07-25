// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InGameController.h"
#include "Objects/ControllableObjectBase.h"

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
        return;
    }

    // 憑依可能な対象を探す
    if (AControllableObjectBase* Target = Cast<AControllableObjectBase>(HitActor))
    {
        OriginalPawn = GetPawn();                     // 現在のPawnを保存
        Possess(Cast<APawn>(Target));                // 対象をPawnとしてキャストして憑依
        bIsPossessing = true;                         // 所有状態フラグをセット
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