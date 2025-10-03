// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TitleController.h"
#include "Blueprint/UserWidget.h"
#include "UI/FadeWidget.h"
#include "UI/MovieWidget.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"

ATitleController::ATitleController()
{
    PrimaryActorTick.bCanEverTick = true;
    IdleCounter = 0.f;
    IdleThreshold = 10.f; // 例: 30秒無操作でムービーへ
    CurrentState = ETitleState::Title;
}

void ATitleController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // ▼ ゲームパッドのすべての主要ボタン（押されたら反応）
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Bottom, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Right, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Left, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Top, IE_Pressed, this, &ATitleController::OnAnyInput);

    InputComponent->BindKey(EKeys::Gamepad_DPad_Up, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_DPad_Down, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_DPad_Left, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_DPad_Right, IE_Pressed, this, &ATitleController::OnAnyInput);

    InputComponent->BindKey(EKeys::Gamepad_LeftShoulder, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_RightShoulder, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_Special_Left, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_Special_Right, IE_Pressed, this, &ATitleController::OnAnyInput);

    // ▼ スティック＆トリガーは Axis なので別処理
    InputComponent->BindAxis("Gamepad_LeftX", this, &ATitleController::OnAnyAxisInput);
    InputComponent->BindAxis("Gamepad_LeftY", this, &ATitleController::OnAnyAxisInput);
    InputComponent->BindAxis("Gamepad_RightX", this, &ATitleController::OnAnyAxisInput);
    InputComponent->BindAxis("Gamepad_RightY", this, &ATitleController::OnAnyAxisInput);
    InputComponent->BindAxis("Gamepad_LeftTriggerAxis", this, &ATitleController::OnAnyAxisInput);
    InputComponent->BindAxis("Gamepad_RightTriggerAxis", this, &ATitleController::OnAnyAxisInput);
}

void ATitleController::OnAnyAxisInput(float Value)
{
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) // 少しでも動かされたら
    {
        OnAnyInput();
    }
}


void ATitleController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentState == ETitleState::Title)
    {
        IdleCounter += DeltaSeconds;
        if (IdleCounter > IdleThreshold)
        {
            CurrentState = ETitleState::FadingToMovie;
            ShowFade(5.f, FSimpleDelegate::CreateUObject(this, &ATitleController::StartMovie));
            SetIgnoreMoveInput(true);
            SetIgnoreLookInput(true);
            IdleCounter = 0;
        }
    }
}

void ATitleController::OnAnyInput()
{
    IdleCounter = 0.f; // リセット

    if (CurrentState == ETitleState::InMovie)
    {
        CurrentState = ETitleState::FadingToTitle;
        //ShowFade(2.f, FSimpleDelegate::CreateUObject(this, &ATitleController::EndMovie));
        EndMovie();

        ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
        if (levelManager == nullptr)
            return;
        UUIManager* uiManager = ALevelManager::GetInstance(GetWorld())->GetUIManager();
        if (uiManager == nullptr)
            return;
        //UUserWidget* widget = ALevelManager::GetInstance(GetWorld())->GetUIManager()->ShowWidget(EWidgetCategory::Menu, "Fade");

        uiManager->ShowWidget(EWidgetCategory::Menu, "Menu");

    }
}

void ATitleController::StartMovie()
{
    ShowMovie();
    CurrentState = ETitleState::InMovie;
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
}

void ATitleController::EndMovie()
{
    HideMovie();
    CurrentState = ETitleState::Title;
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
}

void ATitleController::ShowFade(float Duration, FSimpleDelegate OnFinished)
{
    ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
    if (levelManager == nullptr)
        return;
    UUIManager* uiManager = ALevelManager::GetInstance(GetWorld())->GetUIManager();
    if (uiManager == nullptr)
        return;
    UUserWidget* widget = ALevelManager::GetInstance(GetWorld())->GetUIManager()->ShowWidget(EWidgetCategory::Menu, "Fade");
    if (widget == nullptr)
        return;
    if (UFadeWidget* fadeWidget = Cast<UFadeWidget>(widget))
    {
        fadeWidget->OnFadeFinished = OnFinished;
        fadeWidget->PlayFade(5.f, FSimpleDelegate::CreateUObject(this, &ATitleController::StartMovie));
    }

    uiManager->HideCurrentWidget(EWidgetCategory::Menu, "Menu");
    uiManager->HideCurrentWidget(EWidgetCategory::Menu, "StageSelect");
    IdleCounter = 0;
}


void ATitleController::ShowMovie()
{
    ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
    if (levelManager == nullptr)
        return;
    UUIManager* uiManager = ALevelManager::GetInstance(GetWorld())->GetUIManager();
    if (uiManager == nullptr)
        return;
    UUserWidget* widget = ALevelManager::GetInstance(GetWorld())->GetUIManager()->ShowWidget(EWidgetCategory::Menu, "Demo");
    if (widget == nullptr)
        return;
   
    if (UMovieWidget* movieWidget = Cast<UMovieWidget>(widget))
    {
        movieWidget->PlayMovie();
    }
}

void ATitleController::HideMovie()
{
    ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
    if (levelManager == nullptr)
        return;
    UUIManager* uiManager = ALevelManager::GetInstance(GetWorld())->GetUIManager();
    if (uiManager == nullptr)
        return;
    uiManager->HideCurrentWidget(EWidgetCategory::Menu, "Fade");
    uiManager->HideCurrentWidget(EWidgetCategory::Menu, "Demo");
}
