// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/ColorTargetType.h"
#include "ColorControllerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnColorChanged, FLinearColor, NewColor, EColorTargetType, TargetType);


// C++コード内専用（高速で軽量）
DECLARE_DELEGATE_OneParam(FOnColorChangedNative, FLinearColor);

UENUM(BlueprintType)
enum class EColorChannel : uint8
{
	R UMETA(DisplayName = "Red"),
	G UMETA(DisplayName = "Green"),
	B UMETA(DisplayName = "Blue")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UColorControllerComponent();

    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    UFUNCTION(BlueprintCallable)
    void AdjustColor(EColorChannel Channel, float Delta);

    UFUNCTION(BlueprintCallable)
    FLinearColor GetCurrentColor() const { return CurrentColor; }

    // モードを変更する関数
    void ChangeMode(int Direction);
public:
    UPROPERTY(BlueprintAssignable)
    FOnColorChanged OnColorChanged;

    // シングルキャスト用
    FOnColorChangedNative OnColorChangedNative;

private:
    EColorTargetType GetNextMode(EColorTargetType CurrentMode);
    // 前のモードを取得
    EColorTargetType GetPreviousMode(EColorTargetType CurrentMode);

private:
    // 現在の色付けモード（エディタで編集可能）
    UPROPERTY(EditAnywhere)
    EColorTargetType Mode;

    FLinearColor CurrentColor = FLinearColor::White;
};