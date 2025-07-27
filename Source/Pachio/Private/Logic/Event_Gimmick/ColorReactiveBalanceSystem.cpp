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
    //if (!mLeftPlate || !mRightPlate || !ColorReactiveComponent)
    //    return;

    //// �����̐ݒ�F (Color) �� InColor �̋�����擾
    //float distToTarget = GetColorDistance(InColor, CurrentColor);

    //// ���]�F��擾���ċ��������
    //FLinearColor ComplementaryColor = ColorReactiveComponent->GetComplementaryColor(InColor);
    //float distToComplement = GetColorDistance(ComplementaryColor, CurrentColor);

    //// �ő勗�� = sqrt(3) (RGB�̋����̍ő�l)
    //const float maxDist = FMath::Sqrt(3.0f);

    //// �����𐳋K���i0?1�j
    //float normDistTarget = FMath::Clamp(distToTarget / maxDist, 0.0f, 1.0f);
    //float normDistComplement = FMath::Clamp(distToComplement / maxDist, 0.0f, 1.0f);

    //// �������߂��قǑ傫���グ��C���[�W�Ŕ䗦����
    //// 0�i�����j��1�i�߂��j�ɕϊ�
    //float targetRatio = 1.0f - normDistTarget;
    //float complementRatio = 1.0f - normDistComplement;

    //// �����łǂ��炪��������r���ĎM�̏グ�����ʂ���߂�
    //if (targetRatio >= complementRatio)
    //{
    //    // �F���߂�����InColor�i�E�M��グ��C���[�W�j
    //    FVector rightPos = mRightPlate->GetMaxPosition();
    //    rightPos.Z *= targetRatio;
    //    mRightPlate->SetLocation(rightPos);

    //    FVector leftPos = mLeftPlate->GetMinPosition();
    //    leftPos.Z *= (1.0f - targetRatio);
    //    mLeftPlate->SetLocation(leftPos);
    //}
    //else
    //{
    //    // �F���߂��������]�F�i���M��グ��C���[�W�j
    //    FVector leftPos = mLeftPlate->GetMaxPosition();
    //    leftPos.Z *= complementRatio;
    //    mLeftPlate->SetLocation(leftPos);

    //    FVector rightPos = mRightPlate->GetMinPosition();
    //    rightPos.Z *= (1.0f - complementRatio);
    //    mRightPlate->SetLocation(rightPos);
    //}
}


float AColorReactiveBalanceSystem::GetColorDistance(const FLinearColor& A, const FLinearColor& B) const
{
    return FVector(A.R - B.R, A.G - B.G, A.B - B.B).Length();
}