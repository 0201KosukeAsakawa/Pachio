// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Event_Gimmick/ColorReactiveBalanceSystem.h"
#include "Objects/Color/ColorReactiveBalancePlate.h"
#include "Kismet/KismetMathLibrary.h"

AColorReactiveBalanceSystem::AColorReactiveBalanceSystem()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AColorReactiveBalanceSystem::BeginPlay()
{
    Super::BeginPlay();
}

void AColorReactiveBalanceSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 毎フレーム動かす必要があれば ApplyColorReaction をここで呼ぶ
}

void AColorReactiveBalanceSystem::ApplyColorReaction(const FLinearColor& InColor)
{
    if (!mLeftPlate || !mRightPlate)
        return;

    float distToTarget = GetColorDistance(InColor, TargetColor);
    float distToComplement = GetColorDistance(InColor, GetComplementaryColor(TargetColor));

    float maxDist = FMath::Sqrt(3.0f); // RGB間の最大距離
    float targetScore = 1.0f - (distToTarget / maxDist);
    float complementScore = 1.0f - (distToComplement / maxDist);

    float totalScore = targetScore + complementScore;
    if (totalScore == 0.0f)
    {
        mLeftPlate->ResetLocation();
        mRightPlate->ResetLocation();
        return;
    }

    float rightRatio = targetScore / totalScore;
    float leftRatio = complementScore / totalScore;

    mRightPlate->SetLocation(mRightPlate->GetMinPosition() * rightRatio);
    mLeftPlate->SetLocation(mLeftPlate->GetMinPosition() * leftRatio);
}

float AColorReactiveBalanceSystem::GetColorDistance(const FLinearColor& A, const FLinearColor& B) const
{
    return FVector(A.R - B.R, A.G - B.G, A.B - B.B).Length();
}

FLinearColor AColorReactiveBalanceSystem::GetComplementaryColor(const FLinearColor& InColor) const
{
    return FLinearColor(1.0f - InColor.R, 1.0f - InColor.G, 1.0f - InColor.B, InColor.A);
}