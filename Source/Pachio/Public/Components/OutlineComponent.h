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

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UStaticMeshComponent* OutlineMesh;
    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* OutlineMaterial;
};