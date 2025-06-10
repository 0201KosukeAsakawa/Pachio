// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// UCameraHandlerComponent.cpp

UCameraHandlerComponent::UCameraHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

void UCameraHandlerComponent::BeginPlay()
{
}

void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
    SpringArm->SetupAttachment(RootComponent);
    Camera->SetupAttachment(SpringArm);

    if (Camera)
    {
        PreviousCameraY = Camera->GetComponentLocation().Y;
    }
}

void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCameraPosition(DeltaTime);
}

void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
    if (!Camera)
        return;

    float CurrentY = Camera->GetComponentLocation().Y;
    float DeltaY = CurrentY - PreviousCameraY;

    const float Threshold = 1.0f; // 動きとみなす閾値

    // 右方向の動きを DeltaY < -Threshold とみなす（反転）
    if (DeltaY < -Threshold)
    {
        //SetMovingRight(true);
        //SetMovingLeft(false);
    }
    // 左方向の動きを DeltaY > Threshold とみなす（反転）
    else if (DeltaY > Threshold)
    {
        //SetMovingRight(false);
        //SetMovingLeft(true);
    }
    else
    {
        //SetMovingRight(false);
        //SetMovingLeft(false);
    }

    PreviousCameraY = CurrentY;
}
