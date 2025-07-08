// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ColorFilterInterface.h"

// Add default functionality here for any IColorFilterInterface functions that are not pure virtual.

void IColorReactiveInterface::ColorAction(FLinearColor)
{
}

void IColorReactiveInterface::SetColor(FLinearColor)
{

}

void IColorReactiveInterface::ResetColor()
{
}

bool IColorReactiveInterface::IsColorChange() const
{
    return true;
}

bool IColorReactiveInterface::IsColorModifiable() const
{
    return false;
}

FName IColorReactiveInterface::GetColorEventID() const
{
    return FName();
}
