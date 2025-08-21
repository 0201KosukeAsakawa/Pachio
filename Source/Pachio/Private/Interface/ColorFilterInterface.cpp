// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ColorFilterInterface.h"

// Add default functionality here for any IColorFilterInterface functions that are not pure virtual.

void IColorReactiveInterface::ColorAction(FLinearColor, FEffectMatchResult)
{
}

void IColorReactiveInterface::SetColor(FLinearColor, FEffectMatchResult)
{

}

void IColorReactiveInterface::ResetColor()
{
}

void IColorReactiveInterface::SetSelectMode(bool)
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
bool IColorReactiveInterface::IsChangeable()const
{
    return false;
}
FName IColorReactiveInterface::GetColorEventID() const
{
    return FName();
}
