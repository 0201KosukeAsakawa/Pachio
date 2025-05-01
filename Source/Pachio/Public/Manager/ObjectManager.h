// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjectManager.generated.h"

/**
 * 
 */
class ABaseBlock;
class UMaterialInterface;

UCLASS(Blueprintable)
class PACHIO_API UObjectManager : public UObject
{
	GENERATED_BODY()

public:
	void DuplicateContentsFrom( UObjectManager* Source);
	void GenerateObject(FString, FString,FVector, FRotator);
private:
	UPROPERTY(EditAnywhere)
	TMap<FString, TSubclassOf<AActor>> FloorActor;
	UPROPERTY(EditAnywhere, Category = "Data")
	TMap<FString, TSoftObjectPtr<UMaterialInterface>> MaterialMap;
};
