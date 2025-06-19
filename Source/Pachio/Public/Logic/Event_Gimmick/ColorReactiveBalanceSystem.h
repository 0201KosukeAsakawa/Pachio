// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorReactiveBalanceSystem.generated.h"

class AColorReactiveBalancePlate;

UCLASS()
class PACHIO_API AColorReactiveBalanceSystem : public AActor
{
	GENERATED_BODY()
	
public:
    AColorReactiveBalanceSystem();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AColorReactiveBalancePlate* mLeftPlate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AColorReactiveBalancePlate* mRightPlate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor TargetColor;

    UFUNCTION(BlueprintCallable)
    void ApplyColorReaction(const FLinearColor& InColor);

private:
    float GetColorDistance(const FLinearColor& A, const FLinearColor& B) const;
    FLinearColor GetComplementaryColor(const FLinearColor& InColor) const;
};