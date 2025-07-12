// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UUIManager;
UCLASS()
class PACHIO_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
public:
    virtual void Tick(float DeltaSeconds) override;

    void SetUIManager(UUIManager* InUIManager) { UIManager = InUIManager; }

protected:
    UPROPERTY()
    UUIManager* UIManager;
};
