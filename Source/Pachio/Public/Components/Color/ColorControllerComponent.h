#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorControllerComponent.generated.h"

// Blueprint からバインド可能な色変更通知デリゲート（対象タイプも含む）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnColorChanged, FLinearColor, NewColor, EColorTargetType, TargetType);

// モード切替時などの演出用デリゲート
DECLARE_DELEGATE_OneParam(FColorAnimationDelegate, float);

class IColorReactiveInterface;

// アクターにアタッチして「色の制御・切り替え」を行うコンポーネント
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // ====== 基本 ======

    /**
     * コンストラクタ（デフォルト値を設定）
     */
    UColorControllerComponent();

    /**
     * 毎フレーム呼ばれる更新処理
     *
     * @param DeltaTime - 経過時間
     * @param TickType - Tick の種類
     * @param ThisTickFunction - Tick 関数情報
     */
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ====== 色操作 ======

    /**
     * 現在のモードの色を Hue を Δ値だけ回転させて変更する
     *
     * @param Delta - Hue を加算・減算する割合（-1.0f ~ +1.0f）
     */
    UFUNCTION(BlueprintCallable)
    void AdjustColor(float Delta);

    /**
     * Hue を直接指定して色を決定する
     *
     * @param Value - Hue 値（0 ~ 360）
     */
    UFUNCTION(BlueprintCallable)
    void SetColor(float Value);

    /**
     * 現在のモードで保持している色を取得
     *
     * @return 現在のモードの色
     */
    UFUNCTION(BlueprintCallable)
    FLinearColor GetCurrentColor() const { return ColorMap[CurrentColorMode]; }

    // ====== モード操作 ======

    /**
     * 色モードを切り替える
     *
     * @param Direction - +1なら次のモードへ、-1なら前のモードへ
     */
    void ChangeMode(int Direction);

public:
    // ====== デリゲート ======

    /**
     * Blueprint から購読可能な「色変更通知イベント」
     * （変更後の色と対象モードを通知）
     */
    UPROPERTY(BlueprintAssignable)
    FOnColorChanged OnColorChanged;

    /**
     * モード切替時などの演出用デリゲート（C++ 側で使用）
     */
    FColorAnimationDelegate AnimationDelegate;

private:
    // ====== 内部処理 ======

    /**
     * ObjectColor モード専用の処理（最も近い対象を検索して切替）
     *
     * @param Direction - モード切替方向
     * @param NextMode - 次に切り替えるモード
     */
    void HandleObjectColorMode(int Direction, EColorTargetType NextMode);

    /**
     * WorldColor, UI などのシンプルモード用処理
     *
     * @param Direction - モード切替方向
     * @param NextMode - 次に切り替えるモード
     */
    void HandleSimpleMode(int Direction, EColorTargetType NextMode);

    /**
     * 最も近い色対象（IColorReactiveInterface 実装アクター）を検索する
     *
     * @param OutTarget - 見つかったターゲットインターフェース
     * @param OutActor - 対象アクター
     *
     * @return 見つかった場合は true
     */
    bool FindClosestColorTarget(IColorReactiveInterface*& OutTarget, AActor*& OutActor);

    /**
     * 次のカラーモードを取得（右回り）
     *
     * @param CurrentMode - 現在のモード
     *
     * @return 次のモード
     */
    EColorTargetType GetNextMode(EColorTargetType CurrentMode);

    /**
     * 前のカラーモードを取得（左回り）
     *
     * @param CurrentMode - 現在のモード
     *
     * @return 前のモード
     */
    EColorTargetType GetPreviousMode(EColorTargetType CurrentMode);

    /**
     * 現在モードの隣のモードを取得（方向付き）
     *
     * @param CurrentMode - 現在のモード
     * @param Direction - +1なら次、-1なら前
     *
     * @return 隣のモード
     */
    EColorTargetType GetAdjacentMode(EColorTargetType CurrentMode, int Direction);

private:
    // ====== 内部データ ======

    /**
     * 各モードごとに保持する色のマップ
     */
    TMap<EColorTargetType, FLinearColor> ColorMap;

    /**
     * 現在のカラーモード（例：WorldColor, ObjectColor など）
     */
    UPROPERTY(EditAnywhere)
    EColorTargetType CurrentColorMode;


    FLinearColor Data;
};
