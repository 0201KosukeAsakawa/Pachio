// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BlockDataContainer.generated.h"

class UBlockState;
/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UBlockDataContainer : public UObject
{
	GENERATED_BODY()
public:

    // block�̃X�e�[�g��ێ�����f�[�^�R���e�i
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TMap<FString, TSubclassOf<UBlockState>> AttackStrategyMap;

    // �X�e�[�g���C���X�^���X�����ĕԂ�
    UBlockState* CreateState(UObject*, FString) const;
};
