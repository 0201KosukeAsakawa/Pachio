// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/BoxComponent.h"



// =======================
// コンストラクタ
// =======================

// デフォルト値を設定
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

// =======================
// 色一致時の処理
// =======================

bool UColorTriggerStopComponent::OnColorMatched(const FLinearColor& FilterColor)
{
    // すでに非表示なら処理不要
    if (bHide) return false;
    AActor* Owner = GetOwner();
    if (Owner == nullptr) return false;

    // オーナーが持つ全コンポーネントを走査
    for (UActorComponent* Component : Owner->GetComponents())
    {
        // 「HideTarget」タグが付与されたコンポーネントを対象
        if (Component->ComponentHasTag("HideTarget"))
        {
            // 描画・衝突を持つコンポーネントなら可視性と衝突を無効化
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(false, false);
                Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }

            // アクティブなコンポーネントなら停止
            if (Component->IsActive())
            {
                Component->Deactivate();
            }

            // Tick を持つ場合は停止
            Component->PrimaryComponentTick.SetTickFunctionEnable(false);
        }
    }

    // 消滅時の演出エフェクト
    PlayAppearEffect();

    // 追加の効果を有効化
    //ActiveEffect(true);

    // 非表示状態に更新
    bHide = true;

    return bHide;
}

// =======================
// 色不一致時の処理
// =======================

bool UColorTriggerStopComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
    // すでに表示状態なら処理不要
    if (!bHide) return false;
    AActor* Owner = GetOwner();
    if (Owner == nullptr) return false;

    // エフェクト停止
    DeactivateAllEffects();

    // オーナーが持つ全コンポーネントを走査
    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->ComponentHasTag("HideTarget"))
        {
            // 描画・衝突を持つコンポーネントなら可視性と衝突を有効化
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(true, true);
                Primitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }

            // アクティブ状態に戻す
            if (Component->IsActive())
            {
                Component->Activate(true);
            }

            // Tick を再開
            Component->PrimaryComponentTick.SetTickFunctionEnable(true);
        }
    }

    // 効果を無効化
    //ActiveEffect(false);

    // 表示状態に更新
    return bHide = false;
}
