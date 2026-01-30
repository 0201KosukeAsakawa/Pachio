#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorControllerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChanged, FLinearColor, NewColor);
DECLARE_DELEGATE_OneParam(FColorAnimationDelegate, float);

class IColorReactive;
class UColorUtilityLibrary;
class UObjectColorComponent;
enum class EColorCategory : uint8;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UColorControllerComponent();
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ====== 色操作 ======
    UFUNCTION(BlueprintCallable)
    void AdjustColor(float Delta);

    UFUNCTION(BlueprintCallable)
    FLinearColor GetCurrentColor() const;

    UObjectColorComponent* GetHitColorComponent(float Distance);

    // ====== Tank切り替え ======
    /**
     * 次のTankに切り替え
     */
    UFUNCTION(BlueprintCallable)
    void SwitchToNextTank();

    /**
     * 前のTankに切り替え
     */
    UFUNCTION(BlueprintCallable)
    void SwitchToPreviousTank();

    /**
     * 現在選択中のTankインデックスを取得
     */
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentTankIndex() const { return CurrentTankIndex; }

    /**
     * 指定したTankの残量を取得
     */
    UFUNCTION(BlueprintCallable)
    int32 GetTankAmount(EColorCategory Category) const;

    // ====== 新規追加:統合されたインタラクション ======
    /**
     * 状態に応じて自動的にペイント/吸収を切り替える
     * Tankに色があれば「吐く(ペイント)」、なければ「吸う(吸収)」
     */
    UFUNCTION(BlueprintCallable)
    void InteractWithObject(UObjectColorComponent* TargetComp);

    /**
     * 現在のモードを取得(デバッグ/UI用)
     * @return true = ペイントモード, false = 吸収モード
     */
    UFUNCTION(BlueprintCallable)
    bool IsInPaintMode() const;

public:
    UPROPERTY(BlueprintAssignable)
    FOnColorChanged OnColorChanged;

    void PaintHitObject(UObjectColorComponent* TargetComp);
    void AbsorbHitObject(UObjectColorComponent* TargetComp);

private:
    /**
     * 有効なTank(残量>0)のリストを取得
     */
    TArray<EColorCategory> GetAvailableTanks() const;

    /**
     * 現在のTankインデックスに基づいて色を更新
     */
    void UpdateColorFromCurrentTank();

    /**
     * Tank使用時の処理(残量を減らし、0になったら次に切り替え)
     */
    void ConsumeTank(EColorCategory Category, int32 Amount = 1);

private:
    FLinearColor CurrentColor;

    // RGB Tank順序(Index 0=Red, 1=Green, 2=Blue)
    TArray<EColorCategory> TankOrder;

    // 現在選択中のTankインデックス
    int32 CurrentTankIndex;

public:
    TMap<EColorCategory, int32> ColorTankMap;
};