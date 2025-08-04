#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorConfigurator.generated.h"

class UColorReactiveComponent;
class UBeatScalerComponent;
class ALevelManager;
class UColorManager;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorConfigurator : public UActorComponent
{
	GENERATED_BODY()

public:
	UColorConfigurator();

	// 初期化処理
	virtual void Init();
	virtual void InitializeColorLogic();
	virtual void RegisterToColorManager();
	virtual void SetupMaterial();

	// ビート検出時のアニメーション
	UFUNCTION()
	virtual void PlayBeatAnimation();

	// 色に応じたアクション
	virtual void ColorAction(FLinearColor InColor);

	// 色操作	
	virtual void ResetColor();
	virtual void SetColor(FLinearColor InColor);
	virtual void ApplyColorToMaterial(FLinearColor InColor);
	void SetCurrentColor(FLinearColor InColor);
	void SetColorMuch(bool bInColorMuch);
	void SetSelectMode(bool bIsSelect);
	void ChangeLock(bool bLock) { bColorVariable = bLock; }

	// 色判定・一致確認
	virtual bool IsColorChange() const;
	virtual bool IsColorChange(FLinearColor InColor) const;
	bool IsColorMuch() const;
	bool IsColorMuch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, float Tolerance = 0.08f) const;
	bool IsColorMuch(const FLinearColor& FilterColor, float Tolerance = 0.08f) const;
	bool CheckColorMuch(const FLinearColor& FilterColor, bool bUseComplementaryColor = false) const;
	bool IsChangeable()const;

	// Getter	
	bool IsColorModifiable() const { return bSetColor; }
	FLinearColor GetCurrentColor() const { return CurrentColor; }
	FName GetColorEventID() const { return EventID; }
	EColorTargetType GetColorTargetType() const { return ColorTargetType; }

private:
	// 状態取得
	UStaticMeshComponent* GetStaticMesh() const;
	ALevelManager* GetLevelManager() const;
	UColorManager* GetColorManager() const;
protected:
	// --- Component References ---
	UPROPERTY()
	UBeatScalerComponent* BeatScalerComponent;

	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;

	UPROPERTY()
	UColorReactiveComponent* ColorReactiveComponent;

	// --- Color Data ---
	UPROPERTY(EditAnywhere)
	FLinearColor StartColor;

	UPROPERTY(VisibleAnywhere, Category = "Color")
	FLinearColor CurrentColor;

	// --- Settings ---
	UPROPERTY(EditAnywhere, Category = "Color")
	EColorTargetType ColorTargetType;	
	
	UPROPERTY(EditAnywhere, Category = "Color")
	FName EventID;

	UPROPERTY(EditAnywhere, Category = "Color")
	bool bColorVariable = false;

	UPROPERTY(EditAnywhere, Category = "Color")
	bool bColorChangeable = true;

	UPROPERTY(EditAnywhere, Category = "Color")
	bool bSetColor = true;

	UPROPERTY(EditAnywhere, Category = "Color")
	bool bPlayColorAction = true;

	UPROPERTY(EditAnywhere, Category = "Color")
	bool bUseComplementaryColor = false;

	UPROPERTY(VisibleAnywhere, Category = "Color")
	bool bColorMuch = false;

private:
	bool bIsSelected = false;
};