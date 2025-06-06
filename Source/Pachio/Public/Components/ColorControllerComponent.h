// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorControllerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChanged, FLinearColor, NewColor);

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

    UPROPERTY(BlueprintAssignable)
    FOnColorChanged OnColorChanged;

    // シングルキャスト用
    FOnColorChangedNative OnColorChangedNative;

    UFUNCTION(BlueprintCallable)
    void AdjustColor(EColorChannel Channel, float Delta);

    UFUNCTION(BlueprintCallable)
    FLinearColor GetCurrentColor() const { return CurrentColor; }

private:
    FLinearColor CurrentColor = FLinearColor::White;
};