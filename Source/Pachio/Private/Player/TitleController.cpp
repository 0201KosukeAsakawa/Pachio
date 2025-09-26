// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TitleController.h"
#include "Blueprint/UserWidget.h"
#include "UI/FadeWidget.h"
#include "UI/MovieWidget.h"

ATitleController::ATitleController()
{
    PrimaryActorTick.bCanEverTick = true;
    LastInputTime = 0.f;
    IdleThreshold = 30.f; // 30秒無操作でムービーへ
    CurrentState = ETitleState::Title;
}

void ATitleController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // キーボード
    InputComponent->BindKey(EKeys::AnyKey, IE_Pressed, this, &ATitleController::OnAnyInput);

    // ゲームパッド主要ボタン
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Bottom, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Right, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Left, IE_Pressed, this, &ATitleController::OnAnyInput);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Top, IE_Pressed, this, &ATitleController::OnAnyInput);
}


void ATitleController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentState == ETitleState::Title)
    {
        float Now = GetWorld()->GetTimeSeconds();
        if (Now - LastInputTime > IdleThreshold)
        {
            CurrentState = ETitleState::FadingToMovie;
            ShowFade(5.f, FSimpleDelegate::CreateUObject(this, &ATitleController::StartMovie));
            SetIgnoreMoveInput(true);
            SetIgnoreLookInput(true);
        }
    }
}

void ATitleController::OnAnyInput()
{
    LastInputTime = GetWorld()->GetTimeSeconds();

    if (CurrentState == ETitleState::InMovie)
    {
        CurrentState = ETitleState::FadingToTitle;
        ShowFade(2.f, FSimpleDelegate::CreateUObject(this, &ATitleController::EndMovie));
        SetIgnoreMoveInput(true);
        SetIgnoreLookInput(true);
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
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
}

void ATitleController::ShowFade(float Duration, FSimpleDelegate OnFinished)
{
    if (!FadeWidget && FadeWidgetClass)
    {
        FadeWidget = CreateWidget<UFadeWidget>(this, FadeWidgetClass);
        FadeWidget->AddToViewport(100);
    }
    if (FadeWidget)
    {
        FadeWidget->PlayFade(Duration);
    }
}

void ATitleController::ShowMovie()
{
    if (!MovieWidget && MovieWidgetClass)
    {
        MovieWidget = CreateWidget<UMovieWidget>(this, MovieWidgetClass);
        MovieWidget->AddToViewport(50);
    }
    if (MovieWidget)
    {
        MovieWidget->PlayMovie();
    }
}

void ATitleController::HideMovie()
{
    if (MovieWidget)
    {
        MovieWidget->StopMovie();
        MovieWidget->RemoveFromParent();
        MovieWidget = nullptr;
    }
}
