// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportPortal.generated.h"

UCLASS()
class PACHIO_API ATeleportPortal : public AActor
{
    GENERATED_BODY()

public:
    ATeleportPortal();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    // オーバーラップを検知するコリジョン（例：Box）
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;

    // 対応するペアのポータル
    UPROPERTY(EditAnywhere, Category = "Teleport")
    ATeleportPortal* PairPortal;

    // テレポート直後の再発動を防ぐフラグ（オプション）
    UPROPERTY()
    TMap<AActor*, float> LastTeleportTime;

    // クールタイム（秒）
    UPROPERTY(EditAnywhere, Category = "Teleport")
    float TeleportCooldown = 1.0f;
};
