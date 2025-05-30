// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorControllerComponent.generated.h"

// デリゲート宣言（FLinearColorを引数に取る）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChangedSignature, const FLinearColor&, NewColor);


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

    // デリゲート（Blueprintからバインド可能）
    UPROPERTY(BlueprintAssignable, Category = "Color")
    FOnColorChangedSignature OnColorChanged;

    UFUNCTION(BlueprintCallable)
    void AdjustColor(EColorChannel Channel, float Delta);

    UFUNCTION(BlueprintCallable)
    FLinearColor GetCurrentColor() const { return CurrentColor; }

private:
    FLinearColor CurrentColor = FLinearColor::White;
};