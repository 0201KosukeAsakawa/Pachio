// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorProjectile.generated.h"

UCLASS()
class PACHIO_API AColorProjectile : public AActor
{
    GENERATED_BODY()

public:
    AColorProjectile();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    /** 投射物を発射 */
    void Launch(const FVector& Direction, float Speed, const FLinearColor& InColor);

    /** 色を設定 */
    void SetProjectileColor(const FLinearColor& InColor);

protected:
    /** 衝突時のコールバック */
    UFUNCTION(BlueprintCallable)
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

private:
    /** 球体コリジョン */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class USphereComponent* CollisionComponent;

    /** メッシュコンポーネント */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UStaticMeshComponent* MeshComponent;

    /** 移動コンポーネント */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UProjectileMovementComponent* MovementComponent;

    /** ダイナミックマテリアル */
    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaterial;

    /** 保持する色 */
    UPROPERTY()
    FLinearColor ProjectileColor;

    /** 衝突済みフラグ */
    bool bHasHit;
};
