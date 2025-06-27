// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Event_Gimmick/ColorReactiveBalanceSystem.h"
#include "Components/ColorReactiveComponent.h"
#include "Objects/Color/ColorReactiveBalancePlate.h"
#include "Kismet/KismetMathLibrary.h"

AColorReactiveBalanceSystem::AColorReactiveBalanceSystem()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AColorReactiveBalanceSystem::Init()
{
    AColorReactiveObject::Init();
}

void AColorReactiveBalanceSystem::ColorAction(const FLinearColor InColor)
{
    if (!mLeftPlate || !mRightPlate || !ColorReactiveComponent)
        return;

    // 自分の設定色 (Color) と InColor の距離を取得
    float distToTarget = GetColorDistance(InColor, CurrentColor);

    // 反転色を取得して距離も測る
    FLinearColor ComplementaryColor = GetComplementaryColor(InColor);
    float distToComplement = GetColorDistance(ComplementaryColor, CurrentColor);

    // 最大距離 = sqrt(3) (RGBの距離の最大値)
    const float maxDist = FMath::Sqrt(3.0f);

    // 距離を正規化（0?1）
    float normDistTarget = FMath::Clamp(distToTarget / maxDist, 0.0f, 1.0f);
    float normDistComplement = FMath::Clamp(distToComplement / maxDist, 0.0f, 1.0f);

    // 距離が近いほど大きく上げるイメージで比率を作る
    // 0（遠い）→1（近い）に変換
    float targetRatio = 1.0f - normDistTarget;
    float complementRatio = 1.0f - normDistComplement;

    // ここでどちらが強いか比較して皿の上げ下げ量を決める
    if (targetRatio >= complementRatio)
    {
        // 色が近い方がInColor（右皿を上げるイメージ）
        FVector rightPos = mRightPlate->GetMaxPosition();
        rightPos.Z *= targetRatio;
        mRightPlate->SetLocation(rightPos);

        FVector leftPos = mLeftPlate->GetMinPosition();
        leftPos.Z *= (1.0f - targetRatio);
        mLeftPlate->SetLocation(leftPos);
    }
    else
    {
        // 色が近い方が反転色（左皿を上げるイメージ）
        FVector leftPos = mLeftPlate->GetMaxPosition();
        leftPos.Z *= complementRatio;
        mLeftPlate->SetLocation(leftPos);

        FVector rightPos = mRightPlate->GetMinPosition();
        rightPos.Z *= (1.0f - complementRatio);
        mRightPlate->SetLocation(rightPos);
    }
}


float AColorReactiveBalanceSystem::GetColorDistance(const FLinearColor& A, const FLinearColor& B) const
{
    return FVector(A.R - B.R, A.G - B.G, A.B - B.B).Length();
}