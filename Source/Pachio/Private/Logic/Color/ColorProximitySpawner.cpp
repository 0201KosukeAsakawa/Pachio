// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Manager/ColorManager.h"
#include "Manager/LevelManager.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// =======================
// コンストラクタ
// =======================

// デフォルトコンストラクタ
UColorProximitySpawner::UColorProximitySpawner()
{
}

// =======================
// 色一致・不一致イベント
// =======================

// 色が一致したときに呼ばれる
bool UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
    // パーティクル（Niagara）を有効化
    ToggleNiagaraActiveState(true);

    // メッシュを表示状態に切り替え
    OnMesh();

    // 戻り値は任意仕様（ここでは false）
    return false;
}

// 色が不一致だったときに呼ばれる
bool UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    // パーティクル（Niagara）を無効化
    ToggleNiagaraActiveState(false);

    // メッシュを非表示状態に切り替え
    OffMesh();

    // 戻り値は任意仕様（ここでは true）
    return true;
}

// =======================
// 初期化処理
// =======================

void UColorProximitySpawner::Initialize(const FLinearColor& InitialColor, bool bVariable ,AActor* owner)
{
    FLinearColor color =  ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetEffectColor(Effect);

    // 親クラスの初期化呼び出し
    UColorReactiveComponent::Initialize(color,bVariable, owner);

    // 初期状態はエフェクト停止 & 非表示
    ToggleNiagaraActiveState(false);
    bHide = false;
    OffMesh();
}

// =======================
// メッシュ非表示処理
// =======================

void UColorProximitySpawner::OffMesh()
{
    // すでに非表示なら処理しない
    if (bHide) return;
    AActor* Owner = GetOwner();

    if (Owner == nullptr) return;

    // Owner が持つ全コンポーネントを走査
    for (UActorComponent* Component : Owner->GetComponents())
    {
        // 「HideTarget」タグがついているコンポーネントを対象
        if (Component->ComponentHasTag("HideTarget"))
        {
            // 描画系コンポーネントなら可視性と衝突を無効化
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

            // Tick を持っている場合は停止
            Component->PrimaryComponentTick.SetTickFunctionEnable(false);
        }
    }

    // 出現／消滅エフェクトの再生
    PlayAppearEffect();

    // 非表示フラグ更新
    bHide = true;
}

// =======================
// メッシュ表示処理
// =======================

void UColorProximitySpawner::OnMesh()
{
    // すでに表示状態なら処理しない
    if (!bHide) return;
    AActor* Owner = GetOwner();
    if (Owner == nullptr) return;

    // Owner が持つ全コンポーネントを走査
    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->ComponentHasTag("HideTarget"))
        {
            // 描画系コンポーネントなら可視性と衝突を有効化
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(true, true);
                Primitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }

            // アクティブに戻す
            if (Component->IsActive())
            {
                Component->Activate(true);
            }

            // Tick を再開
            Component->PrimaryComponentTick.SetTickFunctionEnable(true);
        }
    }

    // エフェクトを停止
    DeactivateAllEffects();

    // 非表示フラグ更新
    bHide = false;
}
