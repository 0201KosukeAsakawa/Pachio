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
    /** 色を設定 */
    virtual void ApplyColorWithMatching(const FLinearColor& NewColor, const FEffectMatchResult& MatchResult);

    /** 色をリセット */
    virtual void ResetColor(const FEffectMatchResult& MatchResult);

    /** 選択状態を設定 */
    virtual void SetSelected(bool bIsSelected);

    /** 色が変更されているかを確認 */
    virtual bool HasColorChanged() const;

    /** 色変更が可能かを確認 */
    virtual bool IsChangeable() const;

    /** 色が一致しているかを確認 */
    virtual bool IsColorMatched() const;

    /** 色イベントIDを取得 */
    virtual FName GetColorEventID() const;
};
