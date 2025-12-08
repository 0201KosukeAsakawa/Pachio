// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "ColorReactiveMeshComponent.generated.h"


USTRUCT(BlueprintType)
struct FColorMeshData
{
	GENERATED_BODY()

public:

	/** 色カテゴリに対応するメッシュ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	USkeletalMesh* Mesh = nullptr;

	/** メッシュに対応するアニメーションBP */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TSubclassOf<UAnimInstance> AnimBP = nullptr;
};
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveMeshComponent : public UObjectColorComponent
{
public:
	UColorReactiveMeshComponent();

	GENERATED_BODY()

private:
	/**
	 * @brief 色の反応処理
	 * 引数によってメッシュを切り替える
	 *
	 * @param NewColor 新しく適用された色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& NewColor) override;


private:
	/*
	*@brief色に応じて変わるメッシュ
	*/
	UPROPERTY(EditAnywhere, Category = "Appearance")
	TMap<EColorCategory, FColorMeshData> MeshData;
};
