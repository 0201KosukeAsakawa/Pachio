// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ColorFilterInterface.h"
#include "Blueprint/UserWidget.h"
#include "ColorLens.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class PACHIO_API UColorLens : public UUserWidget,public IColorFilterInterface
{
	GENERATED_BODY()
public:
	void NativeConstruct();
private:
	void ColorAction(FLinearColor)override;
protected:
	// �Ⴆ�Ή�ʂ̐F��ς��邽�߂�UI�p�[�c
	UPROPERTY(meta = (BindWidget))
	UImage* FilterColorImage;
};
