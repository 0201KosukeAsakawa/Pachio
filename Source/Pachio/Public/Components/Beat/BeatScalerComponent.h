// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatScalerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UBeatScalerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBeatScalerComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    void PlayBeat();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Scale")
    float AnimationDuration = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Scale")
    FVector ScaleAxis = FVector(1.1f, 1.0f, 1.0f);  // ��FX������1.5�{

private:
    //アニメーションを再生中か
    bool bIsAnimating = false;
    //アニメーションの再生時間
    float AnimationTime = 0.f;
    //もとのおおきさ
    FVector OriginalScale;
};
