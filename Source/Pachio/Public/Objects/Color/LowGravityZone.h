// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
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

	// �����������i�e�N���X��������j
	virtual void Initialize() override;

private:
	void SetPostProcessEffectEnabled(bool bEnable);
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// �F���������F�F��v�Ői�s�����A��F��v�ŋt�����֕ύX
	virtual void ApplyColorWithMatching(const FLinearColor& InColor ,const FEffectMatchResult&) override;
private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* ZoneBox;

	UPROPERTY()
	TSet<AActor*> OverlappingActors;  // ���݉e����󂯂Ă�A�N�^�[

	UPROPERTY(EditAnywhere, Category = "Universe Effects")
	UNiagaraSystem* UniverseSystem;

	UPROPERTY(EditAnywhere)
	float GravityScale = 0.5f;
	UPROPERTY(EditAnywhere)
	float JumpBuff = 1;

	// ���ۂɍĐ�����R���|�[�l���g
	UPROPERTY()
	UNiagaraComponent* UniverseEffect;

	bool b = false;
};
