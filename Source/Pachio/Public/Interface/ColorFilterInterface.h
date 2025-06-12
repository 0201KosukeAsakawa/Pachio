// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
	virtual void ColorAction(const FLinearColor = FLinearColor::White);
	virtual FName GetColorEventID()const;
};
