#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveObject.generated.h"

class UColorReactiveComponent;

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
	inline bool IsColorMuch() const override { return bColorMuch; }
	inline void ChangeLock(bool b) override { bColorLock = b; }

protected:
	virtual void Init();
	void InitializeColorLogic();
	void RegisterToColorManager();
private:
	void SetupMaterial();

protected:
	// コンポーネント設定
	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;	
	
	// カラーリアクティブコンポーネントの実体
	UPROPERTY()
	UColorReactiveComponent* ColorReactiveComponent;

	// オブジェクト固有の色
	UPROPERTY(EditAnywhere)
	FLinearColor Color;

	// 色の対象種別
	UPROPERTY(EditAnywhere, Category = "Color")
	EColorTargetType ColorTargetType;

	// 色ロック（変更不可）
	UPROPERTY(EditAnywhere)
	bool bColorLock = false;

	// 色が一致したかどうかのフラグ
	UPROPERTY(VisibleAnywhere, Category = "Color")
	bool bColorMuch;
};
