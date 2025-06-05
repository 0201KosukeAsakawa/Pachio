// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

UColorProximitySpawner::UColorProximitySpawner()
{
}

void UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
	    AActor* Owner = GetOwner();
    if (!Owner) return;

    UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(Owner, TEXT("StaticMesh"));
    UBoxComponent* Box = UFunctionLibrary::FindComponentByName<UBoxComponent>(Owner, TEXT("Collision"));
    if (!Mesh || !Box) return;

    float Distance = FVector::Dist(FVector(Color.R, Color.G, Color.B), FVector(FilterColor.R, FilterColor.G, FilterColor.B));
    constexpr float MaxTolerance = 0.5f;

    // 透明度を計算（距離が小さいほど透明）
    float Opacity = FMath::Clamp(1.0f-(Distance / MaxTolerance), 0.0f, 1.0f);

    // 動的マテリアル取得（作成は初回だけにしてキャッシュ推奨）
    if (!DynamicMaterialInstance)
    {
        DynamicMaterialInstance = Mesh->CreateAndSetMaterialInstanceDynamic(0);
    }
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("OpacityParam"), Opacity);
    }

    // 非表示にするのは透明度が十分低い場合だけに
    bool bAlmostInvisible = (Opacity < 0.05f);

    Box->SetHiddenInGame(bAlmostInvisible);
    Mesh->SetCollisionEnabled(bAlmostInvisible ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCastShadow(!bAlmostInvisible);

    // 表示は常にON、透明度で制御
    Mesh->SetVisibility(true);
}

void UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    OnColorMatched(FilterColor);
}
