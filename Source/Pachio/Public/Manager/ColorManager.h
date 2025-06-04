#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "ColorManager.generated.h"

class IColorFilterInterface;

// 色付けモードの列挙型
UENUM(BlueprintType)
enum class EColorMode : uint8
{
    Layer      UMETA(DisplayName = "Layer"),      // レイヤー単位で色付け
    Object     UMETA(DisplayName = "Object"),     // オブジェクト単位で色付け
    Background UMETA(DisplayName = "Background")  // 背景に対して色付け
};

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
    TArray<TScriptInterface<IColorFilterInterface>> Instances;
};

// 色管理を行うマネージャークラス
UCLASS(Blueprintable)
class UColorManager : public UObject
{
    GENERATED_BODY()

public:
    // 色付け対象の初期化処理（クラスからインスタンス化など）
    void InitializeTargets();

    // 新しい色を適用する関数
    UFUNCTION()
    void ApplyColor(FLinearColor NewColor);

    // 色付け対象を登録する関数
    void RegisterTarget(EColorMode Mode, TScriptInterface<IColorFilterInterface> Target);

private:
    // 色付け対象クラスのマップ（モードごとに保持、エディタで編集可能）
    UPROPERTY(EditAnywhere)
    TMap<EColorMode, FColorTargetArray> ColorTargetsClass;

    // 色付け対象インスタンスのマップ（モードごとに保持）
    UPROPERTY()
    TMap<EColorMode, FColorTargetInstanceArray> ColorTargets;

    // 現在アクティブなレイヤーの色付け対象インスタンス
    UPROPERTY()
    TScriptInterface<IColorFilterInterface> ActiveLayerTarget;

    // 現在の色付けモード（エディタで編集可能）
    UPROPERTY(EditAnywhere)
    EColorMode Mode;

    // ポストプロセスマテリアル（エディタで設定可能）
    UPROPERTY(EditAnywhere)
    UMaterialInterface* PostProcessMaterial;

    // ポストプロセスマテリアルの動的インスタンス
    UPROPERTY()
    UMaterialInstanceDynamic* PostProcessMID;

    // 現在適用中の色
    FLinearColor CurrentColor;
};
