// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "ColorReactiveBalanceSystem.generated.h"

class AColorReactiveBalancePlate;

UCLASS()
class PACHIO_API AColorReactiveBalanceSystem : public AColorReactiveObject
{
	GENERATED_BODY()
	
public:
    AColorReactiveBalanceSystem();

protected:
    virtual void Init()override;
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AColorReactiveBalancePlate* mLeftPlate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AColorReactiveBalancePlate* mRightPlate;

    UFUNCTION(BlueprintCallable)
    virtual void ColorAction(const FLinearColor InColor = FLinearColor::White) override;

private:
    float GetColorDistance(const FLinearColor& A, const FLinearColor& B) const;
};