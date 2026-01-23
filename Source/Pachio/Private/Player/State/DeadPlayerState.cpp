// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/DeadPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "Interface/StateControllable.h"
#include "Manager/LevelManager.h"
#include "Sound/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/RespawnComponent.h"

UDeadPlayerState::UDeadPlayerState()
    :ElapsedTime(0.f)
{
   
}

bool UDeadPlayerState::OnEnter(APawn* Owner)
{
    if (!Owner) return false;

    PlayerCharacter = Cast<APlayerCharacter>(Owner);


    if (RespawnComponent == nullptr)
    {
        RespawnComponent = Owner->GetComponentByClass<URespawnComponent>();
    }

    // 入力停止
    if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
    {
        PlayerCharacter->DisableInput(PC);

        // 暗転開始
        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->StartCameraFade(
                0.f, 1.f, 1.f, FLinearColor::Black, false, true
            );
        }

        bIsRespawned = false;
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
    if (PlayerCharacter == nullptr)
        return false;

    ElapsedTime += DeltaTime;

    if (ElapsedTime >= SETUP_POSITION_DELAYTIME && !bIsRespawned)
    {
        if (RespawnComponent == nullptr)
            return false;

        // 実際のリスポーン処理はここで呼ぶ
        RespawnComponent->RespawnOwner();
        bIsRespawned = true;
    }


    if (ElapsedTime >= RESPAWN_DELAY)
    {
        // 自作でOwner取るヘルパーとか

        if (IStateControllable* IS = Cast<IStateControllable>(PlayerCharacter))
        {

            IS->ChangeState(EPlayerStateType::Default);
        }
    }


    return true;
}

bool UDeadPlayerState::OnExit(APawn* Owner)
{
        if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
        {
            // 明るく戻す
            if (PC->PlayerCameraManager)
            {
                PC->PlayerCameraManager->StartCameraFade(
                    1.f, 0.f, 1.f, FLinearColor::Black, false, false
                );
            }

            // 入力再開
            PlayerCharacter->EnableInput(PC);
    }

    return true;
}
