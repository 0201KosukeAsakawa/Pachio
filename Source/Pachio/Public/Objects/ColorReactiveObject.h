#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveObject.generated.h"

class UColorReactiveComponent;

struct FHSLColor
{
	float H; // 0.0〜1.0
	float S; // 0.0〜1.0
	float L; // 0.0〜1.0
};
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
	// インターフェース実装
	virtual void ColorAction(FLinearColor InColor) override;
	virtual void SetColor(FLinearColor)override;
	virtual void ResetColor()override;
	bool IsColorLock()const override { return bColorLock; }
	inline bool IsColorMuch() const override { return bColorMuch; }
	inline void ChangeLock(bool b) override { bColorLock = b; }
	inline FName GetColorEventID()const {return EventID;}
protected:
	virtual void Init();
	virtual void InitializeColorLogic();
	virtual void RegisterToColorManager();
	virtual void SetupMaterial();
	virtual void ApplyColorToMaterial(FLinearColor InColor);
	FLinearColor GetComplementaryColor(const FLinearColor& InColor);
protected:
	// コンポーネント設定
	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;	
	
	// カラーリアクティブコンポーネントの実体
	UPROPERTY()
	UColorReactiveComponent* ColorReactiveComponent;

	// オブジェクト固有の色
	UPROPERTY()
	FLinearColor CurrentColor;

	// オブジェクト固有の色
	UPROPERTY(EditAnywhere)
	FLinearColor StartColor;

	// 色の対象種別
	UPROPERTY(EditAnywhere, Category = "Color")
	EColorTargetType ColorTargetType;

	// 色ロック（変更不可）
	UPROPERTY(EditAnywhere)
	bool bColorLock = false;

	UPROPERTY(EditAnywhere)
	bool bColorVariable  = false;
	// 色が一致したかどうかのフラグ
	UPROPERTY(VisibleAnywhere, Category = "Color")
	bool bColorMuch;

	UPROPERTY(EditAnywhere)
	FName EventID;
};
