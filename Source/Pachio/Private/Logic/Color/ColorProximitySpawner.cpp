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
    ChangeColor(FilterColor);
}

void UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    ChangeColor(FilterColor);
}

void UColorProximitySpawner::ChangeColor(const FLinearColor& FilterColor)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(Owner, TEXT("StaticMesh"));
    UBoxComponent* Box = UFunctionLibrary::FindComponentByName<UBoxComponent>(Owner, TEXT("Collision"));
    if (!Mesh || !Box) return;

    // 色の距離計算（RGBのベクトル距離）
    float Distance = FVector::Dist(FVector(Color.R, Color.G, Color.B), FVector(FilterColor.R, FilterColor.G, FilterColor.B));
    constexpr float MaxTolerance = 0.5f;

    // 透明度の最低値（完全透明にならないように）
    constexpr float MinOpacity = 0.0f;

    // 距離を0~1に正規化
    float NormalizedDist = FMath::Clamp(Distance / MaxTolerance, 0.0f, 1.0f);

    // 色が近いほど不透明、遠いほど透明にする
    float TargetOpacity = FMath::Lerp(1.0f, MinOpacity, NormalizedDist);

    // CurrentOpacityはローカル変数で管理
    static float CurrentOpacity = 1.0f;  // 初期値は完全不透明

    // 現在の透明度と目標透明度を補間
    float FadeSpeed = 5.0f; // 補間速度を速くする
    CurrentOpacity = FMath::FInterpTo(CurrentOpacity, TargetOpacity, GetWorld()->GetDeltaSeconds(), FadeSpeed);

    // 動的マテリアルインスタンス取得（初回のみ）
    if (!DynamicMaterialInstance)
    {
        DynamicMaterialInstance = Mesh->CreateAndSetMaterialInstanceDynamic(0);
    }
    if (DynamicMaterialInstance)
    {
        // 計算した透明度（Opacity）をマテリアルに反映
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("OpacityParam"), CurrentOpacity);
    }

    // 透明度が閾値未満なら非表示＆当たり判定OFFにする
    constexpr float InvisibleThreshold = 0.1f;  // 透明度が0.1未満で非表示にする
    constexpr float Buffer = 0.05f;  // 透明度の変化にバッファを追加

    // コリジョンを切り替えるための閾値を設定
    bool bShouldBeInvisible = (CurrentOpacity < InvisibleThreshold - Buffer); // 透明化状態にする閾値
    bool bShouldBeVisible = (CurrentOpacity > InvisibleThreshold + Buffer);   // 再表示状態にする閾値

    // 非表示状態が変わる場合のみコリジョンを切り替える
    static bool bLastInvisibleState = false;

    if (bShouldBeInvisible && !bLastInvisibleState) // 透明化状態に変わった場合
    {
        // 非表示にしてコリジョン無効化
        Box->SetHiddenInGame(true);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->SetCastShadow(false);
        bLastInvisibleState = true;
    }
    else if (bShouldBeVisible && bLastInvisibleState) // 再表示状態に変わった場合
    {
        // 表示にしてコリジョン有効化
        Box->SetHiddenInGame(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Mesh->SetCastShadow(true);
        bLastInvisibleState = false;
    }

    // メッシュの表示/非表示制御
    if (CurrentOpacity < InvisibleThreshold)
    {
        // 透明度が低い場合、メッシュを非表示にしてコリジョンを無効化
        Mesh->SetVisibility(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    else
    {
        // 透明度が十分に高い場合、メッシュを表示してコリジョンを有効化
        Mesh->SetVisibility(true);
        Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

