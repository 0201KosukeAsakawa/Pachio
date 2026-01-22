#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorManager.generated.h"

class IColorReactive;
class UEffectColorRegistry;
class UColorTargetRegistry;


// 色管理を行うマネージャークラス
UCLASS(Blueprintable)
class UColorManager : public UObject
{
    GENERATED_BODY()

public:
    UColorManager();
    // 色付け対象の初期化処理（クラスからインスタンス化など）
    void Init();

    /**
     * 新しい色を適用する関数
     *
     * @param 対象のオブジェクトに渡すカラー
     * @param 渡す対象の種類
     */
    UFUNCTION()
    void ApplyColor(FLinearColor NewColor);
    /**
     * オブジェクトを指定して新しい色を適用する関数
     *
     * @param 渡す対象のID
     * @param 対象のオブジェクトに渡すカラー
     */
    UFUNCTION()
    void ColorEvent(FName EventID, FLinearColor NewColor);

    /**
     *  色付け対象を登録する関数
     * 
     *  @param 登録する対象
     */
    void RegisterTarget(TScriptInterface<IColorReactive> Target);

    /**
     *  色を角度を利用して差を返す関数
     * 
     *  @param 比べる色
     * 
     *  @return 色差
     * 
     *  この引数と比べる対象は現在のレベルの色
     */
    FVector GetColorDistanceRGB(const FLinearColor& A);

    /**
     *  色を角度を利用して差を返す関数
     *
     *  @param 比べる色1
     *  @param 比べる色2
     * 
     *  @return 色差
     *
     *  この引数と比べる対象は現在のレベルの色
     */
    FVector GetColorDistanceRGB(const FLinearColor& A, const FLinearColor& B );

    /**
     *  色を適用・通知するコンポーネントを返す関数
     * 
     *  @return 色を適用・通知するコンポーネント
     *
     *  戻り値がnullptrかどうかを必ず確認すること
     */
    UColorTargetRegistry* GetColorTargetRegistry()const {return ColorTargetRegistry;}

    /**
     *  現在適用されている全体の色を返す関数
     *
     *  @return 現在適用されている色
     *
     */
    FLinearColor GetWorldColor()const;

private:

    void BindController();
    void InitializePostEffect();

private:
    UPROPERTY()
    UEffectColorRegistry* EffectColorRegistry;
    UPROPERTY()
    UColorTargetRegistry* ColorTargetRegistry;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UColorTargetRegistry> ColorTargetRegistryClass;
    //色に反応するオブジェクトに現在の色を通知
    UPROPERTY()
    TArray<TScriptInterface<IColorReactive>> ColorResponseTargets;
};
