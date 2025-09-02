#include "Components/Color/ColorConfigurator.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
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
	ColorReactiveComponent->UpdateColorEffectAndNiagara(StartColor, Effect, Niagaras);

	if (USkeletalMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh")))
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
	StartColor = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetEffectColor(Effect);
	if (USkeletalMeshComponent* Mesh = GetStaticMesh())
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
	if (!bIsPlayBeat)
		return;

	if (BeatScalerComponent)
	{
		BeatScalerComponent->PlayBeat();
	}
}

void UColorConfigurator::ColorAction(FLinearColor NewColor, FEffectMatchResult result)
{
	if (!bPlayColorAction || !ColorReactiveComponent) return;

	if (bColorVariable)
	{
		ApplyColorToMaterial(NewColor);
	}

	bColorMuch = ColorReactiveComponent->CheckColorMatch(result,NewColor, bUseComplementaryColor);
}

void UColorConfigurator::SetColor(FLinearColor NewColor, FEffectMatchResult result)
{
	CurrentColor = NewColor;

	if (bSetColor)
	{
		ApplyColorToMaterial(CurrentColor);
	}

	if (ColorReactiveComponent)
	{
		ColorReactiveComponent->UpdateColorEffectAndNiagara(CurrentColor, result.ClosestEffect , Niagaras);
	}

	if (const UColorManager* ColorManager = GetColorManager())
	{
		ColorAction(ColorManager->GetWorldColor(), result);
	}
}

void UColorConfigurator::ResetColor(FEffectMatchResult result)
{
	SetColor(StartColor, result);
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
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMatch(StartColor);
}

bool UColorConfigurator::IsColorChange(FLinearColor Color) const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMatch(Color);
}

bool UColorConfigurator::CheckColorMatch(FEffectMatchResult result,const FLinearColor& FilterColor, bool buseComplementaryColor) const
{
	return ColorReactiveComponent && ColorReactiveComponent->CheckColorMatch(result,FilterColor, buseComplementaryColor);
}

bool UColorConfigurator::IsColorMatch() const
{
	return bColorMuch;
}

bool UColorConfigurator::IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, float Tolerance) const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMatch(FilterColor, TargetColor, Tolerance);
}

bool UColorConfigurator::IsColorMatch(const FLinearColor& FilterColor, float Tolerance) const
{
	return ColorReactiveComponent && ColorReactiveComponent->IsColorMatch(FilterColor, Tolerance);
}

bool UColorConfigurator::IsChangeable()const
{
	return bColorChangeable;
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

USkeletalMeshComponent* UColorConfigurator::GetStaticMesh() const
{
	return UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh"));
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
