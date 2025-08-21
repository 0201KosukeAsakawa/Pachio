// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorFilterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UColorReactiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACHIO_API IColorReactiveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ColorAction(const FLinearColor , FEffectMatchResult effect);
	virtual void SetColor(FLinearColor, FEffectMatchResult);
	virtual void ResetColor();
	virtual void SetSelectMode(bool);
	virtual bool IsColorChange()const;
	virtual bool IsColorModifiable()const;
	virtual bool IsChangeable()const;
	virtual FName GetColorEventID()const;
};
