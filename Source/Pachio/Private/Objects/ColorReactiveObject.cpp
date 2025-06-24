// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveObject.h"
#include "Components/ColorReactiveComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "FunctionLibrary.h"


AColorReactiveObject::AColorReactiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void AColorReactiveObject::Init()
{
	InitializeColorLogic();
	RegisterToColorManager();
	SetupMaterial();
}

void AColorReactiveObject::InitializeColorLogic()
{
	if (ReactiveComponentClass == nullptr)
		return;

	ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (ColorReactiveComponent == nullptr)
		return;

	ColorReactiveComponent->RegisterComponent();
	ColorReactiveComponent->Activate(true);
	ColorReactiveComponent->SetMyColor(Color);

	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;
	ColorReactiveComponent->Init(Mesh);


}

void AColorReactiveObject::RegisterToColorManager()
{
	ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
	if (LevelManager == nullptr)
		return;
	UColorManager* ColorManager = LevelManager->GetColorManager();
	if (ColorManager == nullptr)
		return;

	ColorManager->RegisterTarget(ColorTargetType, this);

}

void AColorReactiveObject::SetupMaterial()
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(10);

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial == nullptr)
		return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), Color);

}



void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (bColorLock ||ColorReactiveComponent == nullptr )
		return;

	bColorMuch = ColorReactiveComponent->CheckColorMatch(NewColor);
}


void AColorReactiveObject::ApplyColorToMaterial(FLinearColor InColor)
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (!Mesh) return;

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMaterial) return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), InColor);
}
// RGB -> HSL変換（自作または既存関数を使う）
FHSLColor RGBToHSL(const FLinearColor& Color)
{
    float R = Color.R;
    float G = Color.G;
    float B = Color.B;

    float Max = FMath::Max3(R, G, B);
    float Min = FMath::Min3(R, G, B);
    float Delta = Max - Min;

    FHSLColor HSL;
    HSL.L = (Max + Min) / 2.0f;

    if (Delta == 0)
    {
        HSL.H = 0.0f;
        HSL.S = 0.0f;
    }
    else
    {
        HSL.S = (HSL.L < 0.5f) ? (Delta / (Max + Min)) : (Delta / (2.0f - Max - Min));

        if (Max == R)
            HSL.H = (G - B) / Delta + (G < B ? 6.0f : 0.0f);
        else if (Max == G)
            HSL.H = (B - R) / Delta + 2.0f;
        else
            HSL.H = (R - G) / Delta + 4.0f;

        HSL.H /= 6.0f;
    }
    return HSL;
}

// HSL -> RGB変換
FLinearColor HSLToRGB(const FHSLColor& HSL)
{
    float R, G, B;

    if (HSL.S == 0)
    {
        R = G = B = HSL.L; // 無彩色
    }
    else
    {
        auto HueToRGB = [](float p, float q, float t) -> float
            {
                if (t < 0.0f) t += 1.0f;
                if (t > 1.0f) t -= 1.0f;
                if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
                if (t < 1.0f / 2.0f) return q;
                if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
                return p;
            };

        float q = (HSL.L < 0.5f) ? (HSL.L * (1 + HSL.S)) : (HSL.L + HSL.S - HSL.L * HSL.S);
        float p = 2 * HSL.L - q;

        R = HueToRGB(p, q, HSL.H + 1.0f / 3.0f);
        G = HueToRGB(p, q, HSL.H);
        B = HueToRGB(p, q, HSL.H - 1.0f / 3.0f);
    }

    return FLinearColor(R, G, B, 1.0f);
}

FLinearColor AColorReactiveObject::GetComplementaryColor(const FLinearColor& InColor)
{
    FHSLColor hsl = RGBToHSL(InColor);

    // 色相を180度回転
    hsl.H += 0.5f;
    if (hsl.H > 1.0f) hsl.H -= 1.0f;

    // 彩度を控えめに（例: 0.3固定）
    hsl.S = 0.3f;

    // 明度を高めに（例: 0.75固定）
    hsl.L = 0.75f;

    return HSLToRGB(hsl);
}
