//------------------------------------------
// ファイル名：ThrowComponent.h
// 処理内容：投げる処理
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThrowComponent.generated.h"

//前方宣言
class AThrowingBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UThrowComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	//初期設定
	UFUNCTION(BlueprintCallable, Category = "Throw")
	void Init();

	//ものを投げる関数
	//第1引数：ものを投げる向き
	//第2引数：投げる際にかける力
	UFUNCTION(BlueprintCallable, Category = "Throw")
	virtual void ThrowObject(FVector direction, float force);

	//投げるObjectを決定する関数
	//第1引数：投げるものの詳細
	UFUNCTION(BlueprintCallable, Category = "Throw")
	virtual void SetThrownObject(TSubclassOf<AThrowingBase> thrownObjectClass);

protected:
	//投げられるものクラス（UThrowingBase）を持つ変数
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf <AThrowingBase> ThrownObjectClass;

private:

	UPROPERTY()
	AActor* mOwner = nullptr;
	
};
