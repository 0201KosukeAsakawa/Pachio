// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataContainer/ColorTargetTypes.h"
#include "UObject/NoExportTypes.h"
#include "WeatherEffectManager.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear      UMETA(DisplayName = "Clear"),    /**< 晴れ */
    Rain       UMETA(DisplayName = "Rain"),     /**< 雨 */
    Thunder    UMETA(DisplayName = "Thunder"), /**< 雷 */
    Wind       UMETA(DisplayName = "Wind")     /**< 風 */
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UWeatherComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /** @brief コンストラクタ。エフェクト初期化準備 */
    UWeatherComponent();

    /** @brief コンポーネント開始時の初期化処理 */
    virtual void BeginPlay() override;

    /**
     * @brief 毎フレーム呼び出される更新処理
     *
     * @param DeltaTime 前フレームからの経過時間
     * @param TickType 更新タイプ
     * @param ThisTickFunction Tick 関数用ハンドル
     */
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

    /**
     * @brief 天候と色を指定して天候エフェクトを変更する
     *
     * @param Mode 対象のカラーターゲットタイプ
     * @param NewColor 設定する色
     */
    UFUNCTION(BlueprintCallable)
    void SetWeather(FLinearColor NewColor);

protected:

    /** @brief 雨エフェクト用 Niagara システム */
    UPROPERTY(EditAnywhere, Category = "Weather Effects")
    UNiagaraSystem* RainSystem;

    /** @brief 雷エフェクト用 Niagara システム */
    UPROPERTY(EditAnywhere, Category = "Weather Effects")
    UNiagaraSystem* ThunderSystem;

    /** @brief 風エフェクト用 Niagara システム */
    UPROPERTY(EditAnywhere, Category = "Weather Effects")
    UNiagaraSystem* WindSystem;

private:

    /** @brief 雨エフェクトの Niagara コンポーネント */
    UPROPERTY()
    UNiagaraComponent* RainEffect;

    /** @brief 雷エフェクトの Niagara コンポーネント */
    UPROPERTY()
    UNiagaraComponent* ThunderEffect;

    /** @brief 風エフェクトの Niagara コンポーネント */
    UPROPERTY()
    UNiagaraComponent* WindEffect;

    /** @brief 現在の天候状態 */
    EWeatherType CurrentWeather = EWeatherType::Clear;

    /** @brief エフェクトの初期化処理を行う */
    void InitializeEffects();
};