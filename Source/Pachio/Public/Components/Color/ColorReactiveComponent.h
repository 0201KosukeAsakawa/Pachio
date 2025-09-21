// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveComponent.generated.h"


class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;

struct FHSLColor
{
	float H; // 0.0?1.0
	float S; // 0.0?1.0
	float L; // 0.0?1.0
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UColorReactiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UColorReactiveComponent();

public:
	virtual void Init(UMeshComponent* mesh, bool);
	void UpdateColorEffectAndNiagara(const FLinearColor& FilterColor, EBuffEffect, TArray<ANiagaraActor*>);
	void ApplyColorToMaterial(FLinearColor InColor);
	bool CheckColorMatch(FEffectMatchResult, const FLinearColor& FilterColor, const bool buseComplementaryColor = false);
	UFUNCTION(BlueprintCallable)
	virtual bool IsColorMatch(const FLinearColor& FilterColor, const float Tolerance = 0.08f) const;
	bool IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance = 0.08f) const;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	FLinearColor GetComplementaryColor(const FLinearColor& InColor);
	void SetSelectMode(bool);

	inline bool IsHidden()const {return bHide; }

	void ActiveEffect(bool);
protected:
	void ToggleNiagaraActiveState(bool);
	void PlayAppearEffect();
	void PlayDisappearEffect();
	void ActiveNiagaraEffect(UNiagaraSystem*);
	void DeactivateAllEffects();
private:
	UFUNCTION(BlueprintCallable)
	virtual bool OnColorMatched(const FLinearColor& FilterColor);
	virtual bool OnColorMismatched(const FLinearColor& FilterColor);
protected:
	UPROPERTY(EditAnywhere)
	bool bSetStartColor = true;

	EBuffEffect Effect;

	UPROPERTY(EditAnywhere)
	FLinearColor CurrentColor;

	UPROPERTY()
	TArray<ANiagaraActor*> Niagaras;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMesh;

	UPROPERTY()
	UNiagaraSystem* FireflyBurstNiagaraSystem = nullptr;

	UPROPERTY()
	UNiagaraSystem* ParticlesOfLightNiagaraSystem = nullptr;

	UPROPERTY()
	UNiagaraSystem* LightCubeNiagaraSystem = nullptr;

	UPROPERTY()
	TArray<UNiagaraComponent*> ActiveNiagaraComponent;

	bool bSelected = false;

	bool bHide = false;
};
