// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataContainer.generated.h"

// 前方宣言
class UItemEffectSourceComponent;
class AItemBase;

UCLASS(Blueprintable)
class PACHIO_API UItemDataContainer : public UObject
{
    GENERATED_BODY()

public:

    // CreateState関数: 指定された名前でアイテムのエフェクトソースコンポーネントを生成
    UItemEffectSourceComponent* CreateState(UObject*, FString) const;

    // CreateMaterial関数: 指定された名前に基づいてマテリアルを生成
    UMaterialInterface* CreateMaterial(UObject* WorldContext, FString StateName);

    // GenerateItem関数: アイテムを生成し、初期化する
    AItemBase* GenerateItem(
        FString ItemID,
        FVector location,
        FVector direction,
        const float force = 0,
        const FVector addDirection = FVector(0, 0, 0),
        const FVector scale = FVector(1, 1, 1),
        const FRotator rotation = FRotator(0, 0, 0),
        const FString meshID = "None",
        const FString materialID = "None"
    );

    // アイテムのエフェクトソースコンポーネントクラスを格納するマップ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSubclassOf<UItemEffectSourceComponent>> ItemClassMap;

    // アイテムのマテリアルを格納するマップ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> MaterialMap;

    // アイテムのクラスを格納する
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TSubclassOf<AItemBase> ItemClass;
};