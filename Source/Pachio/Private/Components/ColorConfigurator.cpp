#include "Components/ColorConfigurator.h"
#include "Components/ColorReactiveComponent.h"
#include "Components/BeatScalerComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"

UColorConfigurator::UColorConfigurator()
{
	BeatScalerComponent = CreateDefaultSubobject<UBeatScalerComponent>(TEXT("BeatScalerComponent"));
}

void UColorConfigurator::Init()
{
	InitializeColorLogic();
	RegisterToColorManager();
	SetupMaterial();

	const TObjectPtr<USoundManager> SoundManager = Cast<USoundManager>(GetLevelManager()->GetSoundManager().GetObject());
	if (!SoundManager) return;

	SoundManager->OnBeatDetected.AddDynamic(this, &UColorConfigurator::PlayBeatAnimation);
}

void UColorConfigurator::InitializeColorLogic()
{
	if (!ReactiveComponentClass) return;

	CurrentColor = StartColor;
	ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (!ColorReactiveComponent) return;

	ColorReactiveComponent->RegisterComponent();
	ColorReactiveComponent->Activate(true);
	ColorReactiveComponent->SetMyColor(StartColor);

	if (UStaticMeshComponent* Mesh = GetStaticMesh())
	{
		ColorReactiveComponent->Init(Mesh);
	}
}

void UColorConfigurator::RegisterToColorManager()
{
	if (UColorManager* ColorManager = GetColorManager())
	{
		ColorManager->RegisterTarget(ColorTargetType, GetOwner());
	}
}

void UColorConfigurator::SetupMaterial()
{
	if (!bSetColor) return;

	if (UStaticMeshComponent* Mesh = GetStaticMesh())
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->SetCustomDepthStencilValue(10);

		if (UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			DynMaterial->SetVectorParameterValue(FName("BaseColor"), StartColor);
		}
	}
}

void UColorConfigurator::PlayBeatAnimation()
{
	if (BeatScalerComponent)
	{
		BeatScalerComponent->PlayBeat();
	}
}

void UColorConfigurator::ColorAction(FLinearColor NewColor)
{
	if (!bPlayColorAction || !ColorReactiveComponent) return;

	if (bColorVariable)
	{
		ApplyColorToMaterial(NewColor);
	}

	bColorMuch = ColorReactiveComponent->CheckColorMuch(NewColor, bUseComplementaryColor);
}

void UColorConfigurator::SetColor(FLinearColor NewColor)
{
	CurrentColor = NewColor;

	if (bSetColor)
	{
		ApplyColorToMaterial(CurrentColor);
	}

	if (ColorReactiveComponent)
	{
		ColorReactiveComponent->SetMyColor(CurrentColor);
	}

	if (const UColorManager* ColorManager = GetColorManager())
	{
		ColorAction(ColorManager->GetWorldColor());
	}
}

void UColorConfigurator::ResetColor()
{
	SetColor(StartColor);
}

void UColorConfigurator::SetCurrentColor(FLinearColor NewColor)
{
	CurrentColor = NewColor;
}

void UColorConfigurator::SetColorMuch(bool bInColorMuch)
{
	bColorMuch = bInColorMuch;
}

void UColorConfigurator::SetSelectMode(bool bInIsSelected)
{
	bIsSelected = bInIsSelected;
	if (ColorReactiveComponent)
	{
		ColorReactiveComponent->SetSelectMode(bIsSelected);
	}
}

bool UColorConfigurator::IsColorChange() const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMuch(StartColor);
}

bool UColorConfigurator::IsColorChange(FLinearColor Color) const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMuch(Color);
}

bool UColorConfigurator::CheckColorMuch(const FLinearColor& FilterColor, bool buseComplementaryColor) const
{
	return ColorReactiveComponent && ColorReactiveComponent->CheckColorMuch(FilterColor, buseComplementaryColor);
}

bool UColorConfigurator::IsColorMuch() const
{
	return bColorMuch;
}

bool UColorConfigurator::IsColorMuch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, float Tolerance) const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMuch(FilterColor, TargetColor, Tolerance);
}

bool UColorConfigurator::IsColorMuch(const FLinearColor& FilterColor, float Tolerance) const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMuch(FilterColor, Tolerance);
}

void UColorConfigurator::ApplyColorToMaterial(FLinearColor InColor)
{
	if (ColorReactiveComponent)
	{
		ColorReactiveComponent->ApplyColorToMaterial(InColor);
	}
}

// =======================
// 補助関数（共通処理）
// =======================

UStaticMeshComponent* UColorConfigurator::GetStaticMesh() const
{
	return UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
}

ALevelManager* UColorConfigurator::GetLevelManager() const
{
	return ALevelManager::GetInstance(GetWorld());
}

UColorManager* UColorConfigurator::GetColorManager() const
{
	const ALevelManager* LevelManager = GetLevelManager();
	return LevelManager ? LevelManager->GetColorManager() : nullptr;
}
