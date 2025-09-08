// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ActionControl/CharacterActionInterfaces.h"

// Add default functionality here for any IControllableMover functions that are not pure virtual.

void IControllableMover::Movement(const FInputActionValue& Value)
{
}

void IControllableJumper::Jump(const FInputActionValue& Value)
{
}

void IControllableAbility::Action(const FInputActionValue& Value)
{
}

void IColorModeController::ChangeColor(float)
{
}

void IColorModeController::ShiftArrayRightColorMode()
{
}

void IColorModeController::ShiftArrayLeftColorMode()
{
}

void IStickAction::OnStickMove(const FInputActionValue& Value)
{
}

void IOptionAction::OpenMenu(const FInputActionValue& Value)
{
}
