#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "DataContainer/EffectMatchResult.h"
#include "DataContainer/ColorTargetType.h"
#include "ColorManager.generated.h"

class IColorReactiveInterface;
class UEffectColorMatcher;
class UColorTargetRegistry;


// 色管理を行うマネージャークラス
UCLASS(Blueprintable)
class UColorManager : public UObject
{
    GENERATED_BODY()

public:
    // 色付け対象の初期化処理（クラスからインスタンス化など）
    void Init();

    // 新しい色を適用する関数
    UFUNCTION()
    void ApplyColor(FLinearColor NewColor, EColorTargetType Mode);

    UFUNCTION()
    void ColorEvent(FName EventID, FLinearColor NewColor);

    void SetColorTarget(IColorReactiveInterface*);
    void ResetColorTarget();

    // 色付け対象を登録する関数
    void RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target);

    FEffectMatchResult GetClosestEffectByHue(const FLinearColor& InputColor);


    FLinearColor GetWorldColor()const;
private:

    void BindController();
    void InitializePostEffect();

private:
    UPROPERTY()
    UEffectColorMatcher* EffectColorMatcher;
    UPROPERTY()
    UColorTargetRegistry* ColorTargetRegistry;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UColorTargetRegistry> ColorTargetRegistryClass;
    //色に反応するオブジェクトに現在の色を通知
    UPROPERTY()
    TMap<EColorTargetType, FColorTargetInstanceArray> ColorResponseTargets;
};
