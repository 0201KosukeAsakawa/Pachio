// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/DeadPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "Interface/StateControllable.h"
#include "Manager/LevelManager.h"
#include "Sound/SoundManager.h"
#include "Kismet/GameplayStatics.h" // �����ǉ�

bool UDeadPlayerState::OnEnter(APawn* Owner, UWorld* World)
{
    if (!Owner) return false;

    if (APlayerCharacter* Player = Cast<APlayerCharacter>(Owner))
    {
        // 入力停止
        if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
        {
            Player->DisableInput(PC);

            // 暗転開始
            if (PC->PlayerCameraManager)
            {
                PC->PlayerCameraManager->StartCameraFade(
                    0.f, 1.f, 1.f, FLinearColor::Black, false, true
                );
            }
        }

        // タイマーリセット
        ElapsedTime = 0.f;
    }

    ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());

    if (levelManager == nullptr)
        return false;

    levelManager->GetSoundManager()->PlaySound(TEXT("SE"), TEXT("Dead"));

    return true;
}


bool UDeadPlayerState::OnUpdate(float DeltaTime)
{
    ElapsedTime += DeltaTime;

    if (ElapsedTime >= RespawnDelay)
    {
        if (APawn* OwnerPawn = Cast<APawn>(GetOwner())) // 自作でOwner取るヘルパーとか
        {
            if (IStateControllable* IS = Cast<IStateControllable>(OwnerPawn))
            {
                IS->ChangeState(EPlayerStateType::Default);
            }
        }
    }

    return true;
}

bool UDeadPlayerState::OnExit(APawn* Owner)
{
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(Owner))
    {
        if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
        {
            // 明るく戻す
            if (PC->PlayerCameraManager)
            {
                PC->PlayerCameraManager->StartCameraFade(
                    1.f, 0.f, 1.f, FLinearColor::Black, false, false
                );
            }

            // 入力再開
            Player->EnableInput(PC);
        }

        // 実際のリスポーン処理はここで呼ぶ
        Player->Respawn();
    }

    return true;
}
