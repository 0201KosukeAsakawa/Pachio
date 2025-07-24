// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorConfigurator.generated.h"

class UColorReactiveComponent;
class UBeatScalerComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorConfigurator : public UActorComponent
{
	GENERATED_BODY()
public:
	UColorConfigurator();
protected:

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void PlayBeatAnimation();
	// インターフェース実装
	virtual void ColorAction(FLinearColor InColor);
	virtual void SetColor(FLinearColor);
	virtual void ResetColor();
	virtual void Init();
	virtual void InitializeColorLogic();
	virtual void RegisterToColorManager();
	virtual void SetupMaterial();
	virtual void ApplyColorToMaterial(FLinearColor InColor);

	void SetColorMuch(bool);
	virtual bool IsColorChange()const;
	virtual bool IsColorChange(FLinearColor)const;

	bool CheckColorMatch(const FLinearColor& FilterColor, const bool buseComplementaryColor = false)const;
	bool IsColorMatch() const;
	bool IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance = 0.08f)const;
	bool IsColorMatch(const FLinearColor& FilterColor, const float Tolerance = 0.08f) const;

	inline void ChangeLock(bool b) { bColorVariable = b; }
	inline bool IsColorModifiable()const { return bSetColor; }

	inline FName GetColorEventID()const { return EventID; }
	inline FLinearColor GetCurrentColor()const { return CurrentColor; }
	inline EColorTargetType GetColorTargetType()const { return ColorTargetType; }
protected:

	UPROPERTY()
	UBeatScalerComponent* BeatScalerComponent;

	// コンポーネント設定
	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;

	// カラーリアクティブコンポーネントの実体
	UPROPERTY()
	UColorReactiveComponent* ColorReactiveComponent;

	// オブジェクトの現在の
	// 色
	UPROPERTY()
	FLinearColor CurrentColor;

	// オブジェクト固有の色
	UPROPERTY(EditAnywhere)
	FLinearColor StartColor;

	// 色の対象種別
	UPROPERTY(EditAnywhere, Category = "Color")
	EColorTargetType ColorTargetType;
	UPROPERTY(EditAnywhere)
	bool bColorVariable = false;
	UPROPERTY(EditAnywhere)
	bool bPlayColorAction = true;
	UPROPERTY(EditAnywhere)
	bool buseComplementaryColor = false;
	UPROPERTY(EditAnywhere)
	bool bSetColor = true;
	// 色が一致したかどうかのフラグ
	UPROPERTY(VisibleAnywhere, Category = "Color")
	bool bColorMuch;
	UPROPERTY(EditAnywhere)
	bool bPlayBeat = true;

	UPROPERTY(EditAnywhere)
	FName EventID;
};
