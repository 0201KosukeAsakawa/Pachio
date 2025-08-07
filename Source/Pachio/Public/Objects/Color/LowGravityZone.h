// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "LowGravityZone.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class PACHIO_API ALowGravityZone : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALowGravityZone();

	// 初期化処理（親クラスも初期化）
	virtual void Init() override;

private:
	void SetPostProcessEffectEnabled(bool bEnable);
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 色反応処理：色一致で進行方向、補色一致で逆方向へ変更
	virtual void ColorAction(const FLinearColor InColor = FLinearColor::White) override;
private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* ZoneBox;

	UPROPERTY()
	TSet<AActor*> OverlappingActors;  // 現在影響を受けてるアクター

	UPROPERTY(EditAnywhere, Category = "Universe Effects")
	UNiagaraSystem* UniverseSystem;

	// 実際に再生するコンポーネント
	UPROPERTY()
	UNiagaraComponent* UniverseEffect;
};
