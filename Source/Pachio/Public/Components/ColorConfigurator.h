// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorConfigurator.generated.h"

class UColorReactiveComponent;
class UBeatScalerComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorConfigurator : public UActorComponent
{
	GENERATED_BODY()
public:
	UColorConfigurator();

public:
	UFUNCTION()
	virtual void PlayBeatAnimation();
	// �C���^�[�t�F�[�X����
	virtual void ColorAction(FLinearColor InColor);
	virtual void SetColor(FLinearColor);
	virtual void ResetColor();
	virtual void Init();
	virtual void InitializeColorLogic();
	virtual void RegisterToColorManager();
	virtual void SetupMaterial();
	virtual void ApplyColorToMaterial(FLinearColor InColor);

	void SetColorMuch(bool);
	virtual bool IsColorChange()const;
	virtual bool IsColorChange(FLinearColor)const;

	bool CheckColorMatch(const FLinearColor& FilterColor, const bool buseComplementaryColor = false)const;
	bool IsColorMatch() const;
	bool IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance = 0.08f)const;
	bool IsColorMatch(const FLinearColor& FilterColor, const float Tolerance = 0.08f) const;

	inline void ChangeLock(bool b) { bColorVariable = b; }
	inline bool IsColorModifiable()const { return bSetColor; }

	inline FName GetColorEventID()const { return EventID; }
	inline FLinearColor GetCurrentColor()const { return CurrentColor; }
	inline EColorTargetType GetColorTargetType()const { return ColorTargetType; }
protected:

	UPROPERTY()
	UBeatScalerComponent* BeatScalerComponent;

	// �R���|�[�l���g�ݒ�
	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;

	// �J���[���A�N�e�B�u�R���|�[�l���g�̎���
	UPROPERTY()
	UColorReactiveComponent* ColorReactiveComponent;

	// �I�u�W�F�N�g�̌��݂�
	// �F
	UPROPERTY()
	FLinearColor CurrentColor;

	// �I�u�W�F�N�g�ŗL�̐F
	UPROPERTY(EditAnywhere)
	FLinearColor StartColor;

	// �F�̑Ώێ��
	UPROPERTY(EditAnywhere, Category = "Color")
	EColorTargetType ColorTargetType;
	UPROPERTY(EditAnywhere)
	bool bColorVariable = false;
	UPROPERTY(EditAnywhere)
	bool bPlayColorAction = true;
	UPROPERTY(EditAnywhere)
	bool buseComplementaryColor = false;
	UPROPERTY(EditAnywhere)
	bool bSetColor = true;
	// �F����v�������ǂ����̃t���O
	UPROPERTY(VisibleAnywhere, Category = "Color")
	bool bColorMuch;
	UPROPERTY(EditAnywhere)
	bool bPlayBeat = true;

	UPROPERTY(EditAnywhere)
	FName EventID;
};
