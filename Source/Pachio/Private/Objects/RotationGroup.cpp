// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/RotationGroup.h"
#include "GameFramework/Actor.h"
#include "Components/Color/ColorConfigurator.h"

// Sets default values
ARotationGroup::ARotationGroup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARotationGroup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARotationGroup::Init()
{
    AColorReactiveObject::Init();
}

// Called every frame
void ARotationGroup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// RGB����HSV�ւ̕ϊ��iHue[0-360], Saturation[0-1], Value[0-1]�j
void RGBtoHSV(const FLinearColor& InColor, float& OutH, float& OutS, float& OutV)
{
    float R = InColor.R;
    float G = InColor.G;
    float B = InColor.B;

    float Max = FMath::Max3(R, G, B);
    float Min = FMath::Min3(R, G, B);
    float Delta = Max - Min;

    OutV = Max;

    if (Delta < KINDA_SMALL_NUMBER)
    {
        OutH = 0.f;
        OutS = 0.f;
        return;
    }

    OutS = Delta / Max;

    if (R == Max)
    {
        OutH = (G - B) / Delta;
    }
    else if (G == Max)
    {
        OutH = 2.f + (B - R) / Delta;
    }
    else
    {
        OutH = 4.f + (R - G) / Delta;
    }

    OutH *= 60.f;
    if (OutH < 0.f)
        OutH += 360.f;
}

void ARotationGroup::ColorAction(const FLinearColor InColor, FEffectMatchResult)
{
    if (ColorConfigurator == nullptr)
        return;

    // ���ݐF �� HSV�ϊ�
    float HueCurrent, SatCurrent, ValCurrent;
    float HueTarget, SatTarget, ValTarget;
    RGBtoHSV(ColorConfigurator->GetCurrentColor(), HueCurrent, SatCurrent, ValCurrent);
    RGBtoHSV(InColor, HueTarget, SatTarget, ValTarget);

    float DeltaHue = HueTarget - HueCurrent;
    if (DeltaHue > 180.f)
        DeltaHue -= 360.f;
    else if (DeltaHue < -180.f)
        DeltaHue += 360.f;

    // �X���[�Y�ɂ���Ȃ� DeltaHue *= 0.2f; �ȂǂŒ�����

    // ��]�K�p�iZ���EYaw��]�j
    FRotator CurrentRotation = GetActorRotation();
    FVector v = FVector(RotationAxis.X * DeltaHue, RotationAxis.Y * DeltaHue, RotationAxis.Z * DeltaHue);
    FRotator DeltaRotation = FRotator(v.X, v.Y, v.Z);
    FRotator NewRotation = CurrentRotation + DeltaRotation;

    SetActorRotation(NewRotation);

    // B�Q�ɍ�����]�����K�p
    UpdateBsRelativeToA(DeltaRotation);  // �����Łu�����v�����n���I

    // �F�̏�ԍX�V
    ColorConfigurator->SetCurrentColor(InColor);
}


void ARotationGroup::UpdateBsRelativeToA(const FRotator& DeltaRotation)
{
    if (TargetArray.Num() == 0)
    {
        return;
    }

    FVector Center = GetActorLocation();

    for (const FTargetData& TargetData : TargetArray)
    {
        AActor* BActor = TargetData.targetActor;
        if (!BActor)
            continue;

        // ���x�𔽉f������]�N�H�[�^�j�I��
        FRotator ScaledRotation = DeltaRotation * TargetData.rotateSpeed;
        FQuat DeltaQuat = ScaledRotation.Quaternion();

        // �ʒu���]
        FVector Relative = BActor->GetActorLocation() - Center;
        FVector Rotated = DeltaQuat.RotateVector(Relative);
        BActor->SetActorLocation(Center + Rotated);

        // ��]�������i�p�x�ύX�j
        if (bShouldRotate)
        {
            FQuat CurrentQuat = BActor->GetActorQuat();
            FQuat NewQuat = DeltaQuat * CurrentQuat;
            BActor->SetActorRotation(NewQuat);
        }
    }
}

