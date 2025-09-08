// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ColorReactionConfigInterface.h"

// Add default functionality here for any IColorReactionConfigInterface functions that are not pure virtual.

bool IColorReactionConfigInterface::IsColorMatch() const
{
	return false;
}

void IColorReactionConfigInterface::ChangeLock(bool b)
{
}

FName IColorReactionConfigInterface::GetColorEventID() const
{
	return FName();
}
