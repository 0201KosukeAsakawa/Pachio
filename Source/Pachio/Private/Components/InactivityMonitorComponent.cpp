// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InactivityMonitorComponent.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/InputSettings.h"
#include "Framework/Application/SlateApplication.h"

UInactivityMonitorComponent::UInactivityMonitorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInactivityMonitorComponent::BeginPlay()
{
    Super::BeginPlay();

    LastInputTime = GetWorld()->GetTimeSeconds();

    SetupInputBinding();
}

void UInactivityMonitorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 入力バインディング解除
    if (FSlateApplication::IsInitialized())
    {
       // FSlateApplication::Get().OnInputKey().Remove(InputKeyDelegateHandle);
    }

    Super::EndPlay(EndPlayReason);
}

void UInactivityMonitorComponent::SetupInputBinding()
{
    // 入力全体のコールバック（Slateレベル）でキー入力を監視
    if (FSlateApplication::IsInitialized())
    {
      //  InputKeyDelegateHandle = FSlateApplication::Get().OnInputKey().AddUObject(this, &UInactivityMonitorComponent::OnAnyInput);
    }
}

void UInactivityMonitorComponent::OnAnyInput(FKey Key)
{
    LastInputTime = GetWorld()->GetTimeSeconds();
}

void UInactivityMonitorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bUseTick)
    {
        CheckInactivity();
    }
}

void UInactivityMonitorComponent::CheckInactivity()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if ((CurrentTime - LastInputTime) >= InactivityTimeThreshold)
    {
        // タイムアウトイベント発火
        OnInactivityTimeout.Broadcast();

        // タイムアウト後は再度タイマーをリセット
        LastInputTime = CurrentTime;
    }
}