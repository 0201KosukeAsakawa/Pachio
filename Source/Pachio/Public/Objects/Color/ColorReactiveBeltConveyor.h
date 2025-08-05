// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "ColorReactiveBeltConveyor.generated.h"

class UBoxComponent;
class UPhysicsCalculator;

/**
 * 色によって挙動（ベルトの進行方向）が変化するベルトコンベアクラス
 * 対象のアクター（UPhysicsCalculator を持つ）に力を加える
 */
UCLASS()
class PACHIO_API AColorReactiveBeltConveyor : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	AColorReactiveBeltConveyor();

	// 初期化処理（親クラスも初期化）
	virtual void Init() override;

	// 毎フレーム呼ばれる処理：ベルト上の物体に力を加える
	virtual void Tick(float) override;

private:
	// 色反応処理：色一致で進行方向、補色一致で逆方向へ変更
	virtual void ColorAction(const FLinearColor InColor = FLinearColor::White) override;

	// オーバーラップ開始処理：対象をベルト上として登録
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// オーバーラップ終了処理：対象の登録解除
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBeatDetected();

private:
	// ベルトの当たり判定用 Box コリジョン
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	// ベルトの進行方向（Editor 設定可能）
	UPROPERTY(EditAnywhere)
	FVector direction;

	// ベルトの推進力（Editor 設定可能）
	UPROPERTY(EditAnywhere)
	float DefaultPower = 0;
	float CurrentPower = 0;
	// 実際の現在方向（色に応じて切り替わる）
	FVector CurrentDirection;

	// ベルト上に存在する対象アクター（物理的に影響を与える）
	UPROPERTY()
	TArray<UPhysicsCalculator*> hitObject;

	UPROPERTY(EditAnywhere)
	bool IsRevers = false;
};
