// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OutlineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UOutlineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOutlineComponent();
    UFUNCTION(BlueprintCallable)
    void SetStencilValue(int32 Value);
    void EnableOutline(bool bEnable);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY(EditAnywhere, Category = "Outline")
    int32 StencilValue = 1;
    UMeshComponent* CachedMesh;

    void InitMesh();
    bool IsBehindWall();
};