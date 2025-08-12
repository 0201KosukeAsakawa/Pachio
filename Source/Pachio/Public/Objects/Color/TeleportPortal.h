// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "TeleportPortal.generated.h"

UCLASS()
class PACHIO_API ATeleportPortal : public AColorReactiveObject
{
    GENERATED_BODY()

public:
    ATeleportPortal();

protected:
    virtual void BeginPlay() override;
    // 初期化処理（親クラスも初期化）
    virtual void Init() override;
    virtual void ColorAction(const FLinearColor InColor,FEffectMatchResult) override;
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    // オーバーラップを検知するコリジョン（例：Box）
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;
    UPROPERTY(EditAnywhere)
    FLinearColor SecondColor;
    // 対応するペアのポータル
    UPROPERTY()
    ATeleportPortal* CurrentTargetPortal;

    UPROPERTY(EditAnywhere, Category = "Teleport")
    ATeleportPortal* PrimaryDestination;

    UPROPERTY(EditAnywhere, Category = "Teleport")
    ATeleportPortal* AlternatePortal;

    UPROPERTY(EditAnywhere, Category = "Teleport")
    TArray<FName> AllowedTags;

    // テレポート直後の再発動を防ぐフラグ（オプション）
    UPROPERTY()
    TMap<AActor*, float> LastTeleportTime;

    // クールタイム（秒）
    UPROPERTY(EditAnywhere, Category = "Teleport")
    float TeleportCooldown = 1.0f;
};
