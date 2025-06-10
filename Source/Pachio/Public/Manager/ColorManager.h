#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "DataContainer/EffectMatchResult.h"
#include "DataContainer/ColorTargetType.h"
#include "ColorManager.generated.h"

class IColorReactiveInterface;



// 色付け対象のクラス群を格納する構造体（編集可能）
USTRUCT(BlueprintType)
struct FColorTargetArray
{
    GENERATED_BODY()

    // 色付け対象のクラス配列
    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<UObject>> Targets;
};

// 実体の色付け対象インスタンス群を格納する構造体
USTRUCT()
struct FColorTargetInstanceArray
{
    GENERATED_BODY()

    // 色付け対象インターフェースを持つインスタンス配列
    TArray<TScriptInterface<IColorReactiveInterface>> Instances;
};

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

    // 色付け対象を登録する関数
    void RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target);

    FEffectMatchResult GetClosestEffectByHue(const FLinearColor& InputColor);

private:
    void InitializeTargets();
    void BindController();
    void InitializePostEffect();
    void NotifyTargets(EColorTargetType Mode, const FLinearColor& Color);
    float GetColorDistanceRGB(const FLinearColor& A, const FLinearColor& B);
private:
    // 色付け対象クラスのマップ（モードごとに保持、エディタで編集可能）
    UPROPERTY(EditAnywhere)
    TMap<EColorTargetType, FColorTargetArray> ColorTargetsClass;

    // 色付け対象インスタンスのマップ（モードごとに保持）
    UPROPERTY()
    TMap<EColorTargetType, FColorTargetInstanceArray> ColorableObjectsMap;

    //色に反応するオブジェクトに現在の色を通知
    UPROPERTY()
    TMap<EColorTargetType, FColorTargetInstanceArray> ColorResponseTargets;

    TMap<EBuffEffect, FLinearColor> EffectColorMap;

    // ポストプロセスマテリアル（エディタで設定可能）
    UPROPERTY(EditAnywhere)
    UMaterialInterface* PostProcessMaterial;

    // ポストプロセスマテリアルの動的インスタンス
    UPROPERTY()
    UMaterialInstanceDynamic* PostProcessMID;
};
