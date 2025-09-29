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

    Player = Cast<APlayerCharacter>(Owner);

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

        bIsRespawn = false;
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
    if (Player == nullptr)
        return false;

    ElapsedTime += DeltaTime;

    if (ElapsedTime >= 1.5f && !bIsRespawn)
    {
        // 実際のリスポーン処理はここで呼ぶ
        Player->Respawn();
        bIsRespawn = true;
    }


    if (ElapsedTime >= RespawnDelay)
    {
        // 自作でOwner取るヘルパーとか

        if (IStateControllable* IS = Cast<IStateControllable>(Player))
        {

            IS->ChangeState(EPlayerStateType::Default);
        }
    }


    return true;
}

bool UDeadPlayerState::OnExit(APawn* Owner)
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

    return true;
}
