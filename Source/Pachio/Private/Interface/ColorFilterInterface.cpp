// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/ColorFilterInterface.h"


/** 色を設定 */
void IColorReactiveInterface::ApplyColorWithMatching(const FLinearColor& NewColor, const FEffectMatchResult& MatchResult)
{

}

/** 色をリセット */
void IColorReactiveInterface::ResetColor(const FEffectMatchResult& MatchResult)
{

}

/** 選択状態を設定 */
void IColorReactiveInterface::SetSelected(bool bIsSelected)
{

}

/** 色が変更されているかを確認 */
bool IColorReactiveInterface::HasColorChanged()const
{
	return false;
}

/** 色変更が可能かを確認 */
bool IColorReactiveInterface::IsChangeable() const
{
	return false;
}

/** 色が一致しているかを確認 */
bool IColorReactiveInterface::IsColorMatched() const
{
	return false;
}

/** 色イベントIDを取得 */
FName IColorReactiveInterface::GetColorEventID() const
{
	return FName();
}