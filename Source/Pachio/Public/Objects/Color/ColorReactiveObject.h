#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveObject.generated.h"

class UColorReactiveComponent;
class UBeatScalerComponent;
class UColorConfigurator;

/**
 * 色に反応するアクター（指定色でアクションを起こす）
 */
UCLASS()
class PACHIO_API AColorReactiveObject : public AActor, public IColorReactiveInterface, public IColorReactionConfigInterface
{
	GENERATED_BODY()

public:
	AColorReactiveObject();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void PlayBeatAnimation();
	// インターフェース実装
	virtual void ColorAction(FLinearColor InColor, FEffectMatchResult) override;
	virtual void SetColor(FLinearColor, FEffectMatchResult result)override;
	virtual void ResetColor()override;
	virtual void SetSelectMode(bool)override;
	virtual void ChangeLock(bool b) override;	
	virtual bool IsColorChange()const override;
	virtual bool IsChangeable()const override;
	virtual bool IsColorModifiable()const override;
	virtual bool IsColorMuch() const override;
	FName GetColorEventID()const override;

protected:
	virtual void Init();
	virtual void InitializeColorLogic();
	virtual void RegisterToColorManager();
	virtual void SetupMaterial();
	virtual void ApplyColorToMaterial(FLinearColor InColor);
protected:
	UPROPERTY(EditAnywhere)
	UColorConfigurator* ColorConfigurator;
	UPROPERTY(EditAnywhere, Category = "Beat")
	bool bPlayBeat = true;
};
