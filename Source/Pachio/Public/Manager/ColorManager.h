#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "ColorManager.generated.h"

class IColorFilterInterface;

// 色モード列挙体
UENUM(BlueprintType)
enum class EColorMode : uint8
{
    Layer      UMETA(DisplayName = "Layer"),
    Object     UMETA(DisplayName = "Object"),
    Background UMETA(DisplayName = "Background")
};

// ブループリントクラスを保持する構造体（エディタ設定用）
USTRUCT(BlueprintType)
struct FColorTargetArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<UObject>> Targets;
};

// 実行時インスタンスを保持する構造体
USTRUCT()
struct FColorTargetInstanceArray
{
    GENERATED_BODY()

    TArray<TScriptInterface<IColorFilterInterface>> Instances;
};

// UColorManagerクラス本体
UCLASS(Blueprintable)
class UColorManager : public UObject
{
    GENERATED_BODY()

public:
    // エディタで設定するブループリントクラス群
    UPROPERTY(EditAnywhere)
    TMap<EColorMode, FColorTargetArray> ColorTargetsClass;

    // 実行時に生成されたインスタンス群
    UPROPERTY()
    TMap<EColorMode, FColorTargetInstanceArray> ColorTargets;

    UPROPERTY()
    TScriptInterface<IColorFilterInterface> ActiveLayerTarget;

    // 現在のモード
    UPROPERTY(EditAnywhere)
    EColorMode Mode;

public:
    // 初期化関数（ブループリントクラスからインスタンスを生成）
    void InitializeTargets();

    // 色を適用する関数
    UFUNCTION()
    void ApplyColor(FLinearColor NewColor);

    // ターゲットを追加登録する関数（必要に応じて）
    void RegisterTarget(EColorMode Mode, TScriptInterface<IColorFilterInterface> Target);
};
