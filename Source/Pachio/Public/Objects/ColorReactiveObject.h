#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveObject.generated.h"

class UColorReactiveComponent;
class UBeatScalerComponent;

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
	virtual void ColorAction(FLinearColor InColor) override;
	virtual void SetColor(FLinearColor)override;
	virtual void ResetColor()override;
	virtual bool IsColorChange()const override;	
	virtual void SetSelectMode(bool)override;
	inline void ChangeLock(bool b) override { bColorVariable = b; }
	inline bool IsColorModifiable()const override { return bSetColor; }
	inline bool IsColorMuch() const override { return bColorMuch; }
	inline FName GetColorEventID()const override{return EventID;}
	
protected:
	virtual void Init();
	virtual void InitializeColorLogic();
	virtual void RegisterToColorManager();
	virtual void SetupMaterial();
	virtual void ApplyColorToMaterial(FLinearColor InColor);
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
	bool bColorVariable  = false;
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
