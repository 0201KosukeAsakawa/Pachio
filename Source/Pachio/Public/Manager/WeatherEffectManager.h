// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataContainer/EffectMatchResult.h"
#include "UObject/NoExportTypes.h"
#include "WeatherEffectManager.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear      UMETA(DisplayName = "Clear"),
    Rain       UMETA(DisplayName = "Rain"),
    Thunder    UMETA(DisplayName = "Thunder"),
    Wind       UMETA(DisplayName = "Wind")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UWeatherComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeatherComponent();

    virtual void BeginPlay() override;

    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

    UFUNCTION(BlueprintCallable)
    void SetWeather(EColorTargetType Mode,FLinearColor NewColor);

protected:
    // 天候用Niagaraアセット（エディタでセット可能）
    UPROPERTY(EditAnywhere, Category = "Weather Effects")
    UNiagaraSystem* RainSystem;

    UPROPERTY(EditAnywhere, Category = "Weather Effects")
    UNiagaraSystem* ThunderSystem;

    UPROPERTY(EditAnywhere, Category = "Weather Effects")
    UNiagaraSystem* WindSystem;

private:
    // 実際に再生するコンポーネント
    UPROPERTY()
    UNiagaraComponent* RainEffect;

    UPROPERTY()
    UNiagaraComponent* ThunderEffect;

    UPROPERTY()
    UNiagaraComponent* WindEffect;

    EWeatherType CurrentWeather = EWeatherType::Clear;

    void InitializeEffects();
};