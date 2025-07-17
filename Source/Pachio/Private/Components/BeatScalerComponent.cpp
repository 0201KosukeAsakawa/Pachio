// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BeatScalerComponent.h"

UBeatScalerComponent::UBeatScalerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBeatScalerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        OriginalScale = Owner->GetActorScale3D();
    }
}

void UBeatScalerComponent::PlayBeat()
{
    if (bIsAnimating) return;

    AnimationTime = 0.f;
    bIsAnimating = true;
}

void UBeatScalerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsAnimating) return;

    AnimationTime += DeltaTime;
    float HalfDuration = AnimationDuration * 0.5f;

    FVector NewScale = OriginalScale;

    if (AnimationTime <= HalfDuration)
    {
        NewScale = FMath::Lerp(OriginalScale, OriginalScale * ScaleAxis, AnimationTime / HalfDuration);
    }
    else if (AnimationTime <= AnimationDuration)
    {
        float Alpha = (AnimationTime - HalfDuration) / HalfDuration;
        NewScale = FMath::Lerp(OriginalScale * ScaleAxis, OriginalScale, Alpha);
    }
    else
    {
        NewScale = OriginalScale;
        bIsAnimating = false;
    }

    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorScale3D(NewScale);
    }
}