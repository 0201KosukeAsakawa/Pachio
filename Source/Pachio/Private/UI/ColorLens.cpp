// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ColorLens.h"
#include "Components/Image.h"

void UColorLens::UpdateFilterColor(const FLinearColor& NewColor)
{
    if (FilterColorImage)
    {
        // UI‚ÌF‚ð•Ï‚¦‚éiSetColorAndOpacity‚ÍUImage‚ÌŠÖ”j
        FilterColorImage->SetColorAndOpacity(NewColor);
    }
}
