// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ColorFilterInterface.h"


/** �F��ݒ� */
void IColorReactiveInterface::ApplyColorWithMatching(const FLinearColor& NewColor)
{

}

/** �F����Z�b�g */
void IColorReactiveInterface::ResetColor()
{

}

/** �I���Ԃ�ݒ� */
void IColorReactiveInterface::SetSelected(bool bIsSelected)
{

}

/** �F���ύX����Ă��邩��m�F */
bool IColorReactiveInterface::HasColorChanged()const
{
	return false;
}

/** �F�ύX���\����m�F */
bool IColorReactiveInterface::IsChangeable() const
{
	return false;
}

/** �F����v���Ă��邩��m�F */
bool IColorReactiveInterface::IsColorMatched() const
{
	return false;
}

/** �F�C�x���gID��擾 */
FName IColorReactiveInterface::GetColorEventID() const
{
	return FName();
}