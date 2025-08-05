#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/ColorTargetType.h"
#include "ColorControllerComponent.generated.h"

// Blueprint からバインド可能な色変更通知デリゲート（対象タイプも含む）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnColorChanged, FLinearColor, NewColor, EColorTargetType, TargetType);

DECLARE_DELEGATE_OneParam(FColorAnimationDelegate, float);

class IColorReactiveInterface;

// アクターにアタッチして色の制御を行うコンポーネント
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // コンストラクタ
    UColorControllerComponent();

    // 毎フレーム呼ばれる更新処理（必要なら使用）
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 指定された色チャンネル（R/G/B）の値を加算・減算して色を調整
    UFUNCTION(BlueprintCallable)
    void AdjustColor(float Delta);

    // 現在の色を取得
    UFUNCTION(BlueprintCallable)
    FLinearColor GetCurrentColor() const { return ColorMap[CurrentColorMode]; }

    // モードを切り替える（+1 / -1 などの方向指定）
    void ChangeMode(int Direction);

    

public:
    // Blueprint から購読可能な色変更イベント（色と対象タイプを通知）
    UPROPERTY(BlueprintAssignable)
    FOnColorChanged OnColorChanged;

    FColorAnimationDelegate AnimationDelegate;

private:
    void HandleObjectColorMode(int Direction, EColorTargetType NextMode);
    void HandleSimpleMode(int Direction, EColorTargetType NextMode);
    bool FindClosestColorTarget(IColorReactiveInterface*& OutTarget, AActor*& OutActor);
    // 次のカラーモードを取得（右回り）
    EColorTargetType GetNextMode(EColorTargetType CurrentMode);

    // 前のカラーモードを取得（左回り）
    EColorTargetType GetPreviousMode(EColorTargetType CurrentMode);

    EColorTargetType GetAdjacentMode(EColorTargetType CurrentMode, int Direction);

private:

    TMap<EColorTargetType, FLinearColor>ColorMap;
    // 現在のカラーモード（どの対象に色を適用するか）
    UPROPERTY(EditAnywhere)
    EColorTargetType CurrentColorMode;
};
