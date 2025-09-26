// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InactivityMonitorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UInactivityMonitorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInactivityMonitorComponent();

    // 無操作で遷移するまでの時間（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inactivity")
    float InactivityTimeThreshold = 300.f; // 5分

    // Tick で監視するか（低頻度チェックにするなら false）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inactivity")
    bool bUseTick = true;

    // タイムアウト時に発火するイベント（Blueprintでも使える）
    UPROPERTY(BlueprintAssignable, Category = "Inactivity")
    FOnInactivityTimeout OnInactivityTimeout;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float LastInputTime = 0.f;
    FDelegateHandle InputKeyDelegateHandle;

    void OnAnyInput(FKey Key);
    void CheckInactivity();
    void SetupInputBinding();
};
