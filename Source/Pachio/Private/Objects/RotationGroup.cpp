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
    // 1. 現在の色を取得（メンバ変数 color と仮定）
    FLinearColor CurrentColor = Color;

    // 2. RGBからHSVに変換
    float HueCurrent, SatCurrent, ValCurrent;
    float HueTarget, SatTarget, ValTarget;

    RGBtoHSV(CurrentColor, HueCurrent, SatCurrent, ValCurrent);
    RGBtoHSV(InColor, HueTarget, SatTarget, ValTarget);

    // 3. 色相差の計算（-180?180度）
    float DeltaHue = HueTarget - HueCurrent;
    if (DeltaHue > 180.f)
        DeltaHue -= 360.f;
    else if (DeltaHue < -180.f)
        DeltaHue += 360.f;

    UE_LOG(LogTemp, Log, TEXT("Hue difference: %f degrees"), DeltaHue);

    // 4. 回転を加算（ここではYaw軸回転）
    FRotator CurrentRotation = GetActorRotation();
    FRotator NewRotation = CurrentRotation + FRotator(0.f, DeltaHue, 0.f);
    SetActorRotation(NewRotation);

    // 5. 関連するオブジェクトの位置も更新
    UpdateBsRelativeToA(NewRotation);

    // 6. 現在の色を更新（オプション）
    Color = InColor;
}


void ARotationGroup::UpdateBsRelativeToA(const FRotator& NewRotation)
{
    if (TargetArray.Num() == 0)
    {
        return;
    }

    FVector Center = GetActorLocation();  // これが回転の中心
    FQuat NewQuat = NewRotation.Quaternion();

    for (AActor* BActor : TargetArray)
    {
        if (!BActor)
            continue;

        FVector RelativePos = BActor->GetActorLocation() - Center;
        FVector RotatedPos = NewQuat.RotateVector(RelativePos);
        FVector NewBLocation = Center + RotatedPos;

        BActor->SetActorLocation(NewBLocation);
    }
}
