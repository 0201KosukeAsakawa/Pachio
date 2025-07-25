// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockonWidget.generated.h"

class UImage;
UCLASS()
class PACHIO_API ULockonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    void SetTargetActor(AActor* InActor) { TargetActor = InActor; }
    AActor* GetTargetActor() const { return TargetActor; }

    void UpdateScreenPosition(const FVector2D& ScreenPosition);

    void SetMarkerVisible(bool bVisible);

protected:
    // UMGÇÃâÊëúÇ…BindÇ∑ÇÈèÍçáÇÕmeta BindWidgetÇégÇ§
    UPROPERTY(meta = (BindWidget))
    UImage* LockonImage;
    UPROPERTY(EditAnywhere)
    FVector2D AddVectorScale = FVector2D(0,0);
private:
    UPROPERTY()
    AActor* TargetActor;
};
