// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/RotationGroup.h"
#include "GameFramework/Actor.h"

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

// RGBからHSVへの変換（Hue[0-360], Saturation[0-1], Value[0-1]）
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

void ARotationGroup::ColorAction(const FLinearColor InColor)
{
    // 現在色 → HSV変換
    float HueCurrent, SatCurrent, ValCurrent;
    float HueTarget, SatTarget, ValTarget;
    RGBtoHSV(Color, HueCurrent, SatCurrent, ValCurrent);
    RGBtoHSV(InColor, HueTarget, SatTarget, ValTarget);

    float DeltaHue = HueTarget - HueCurrent;
    if (DeltaHue > 180.f)
        DeltaHue -= 360.f;
    else if (DeltaHue < -180.f)
        DeltaHue += 360.f;

    // スムーズにするなら DeltaHue *= 0.2f; などで調整可

    // 回転適用（Z軸・Yaw回転）
    FRotator CurrentRotation = GetActorRotation();
    FVector v = FVector(RotationAxis.X * DeltaHue, RotationAxis.Y * DeltaHue, RotationAxis.Z * DeltaHue);
    FRotator DeltaRotation = FRotator(v.X, v.Y, v.Z);
    FRotator NewRotation = CurrentRotation + DeltaRotation;

    SetActorRotation(NewRotation);

    // B群に差分回転だけ適用
    UpdateBsRelativeToA(DeltaRotation);  // ここで「差分」だけ渡す！

    // 色の状態更新
    Color = InColor;
}


void ARotationGroup::UpdateBsRelativeToA(const FRotator& DeltaRotation)
{
    if (TargetArray.Num() == 0)
    {
        return;
    }

    FVector Center = GetActorLocation();
    FQuat DeltaQuat = DeltaRotation.Quaternion();

    for (AActor* BActor : TargetArray)
    {
        if (!BActor)
            continue;

        // 位置を回転
        FVector Relative = BActor->GetActorLocation() - Center;
        FVector Rotated = DeltaQuat.RotateVector(Relative);
        BActor->SetActorLocation(Center + Rotated);

        // 回転も加える（角度変更）
        if (bShouldRotate)
        {
            FQuat CurrentQuat = BActor->GetActorQuat();
            FQuat NewQuat = DeltaQuat * CurrentQuat;
            BActor->SetActorRotation(NewQuat);
        }
    }
}
