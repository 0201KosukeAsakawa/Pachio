// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "ColorTriggerStopComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UColorTriggerStopComponent : public UColorReactiveComponent
{
    GENERATED_BODY()

public:
    UColorTriggerStopComponent();

   // virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    bool OnColorMatched(const FLinearColor& FilterColor);
    bool OnColorMismatched(const FLinearColor& FilterColor);


};
