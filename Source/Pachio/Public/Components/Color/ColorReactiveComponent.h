// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorUtilityLibrary.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveComponent.generated.h"


class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UColorReactiveComponent();

    // =======================
    // 初期化
    // =======================

    /**
     * 色とマテリアルの初期化
     * @param InitialColor 初期色
     * @param bVariable 色を変数として扱うか（動的変更可能）
     * @param Owner オーナーアクター（nullptrの場合はGetOwner()を使用）
     */
    virtual void Initialize(const FLinearColor& InitialColor, bool bVariable, AActor* Owner = nullptr);

    /** Niagaraアクターの設定 */
    void SetupNiagaraActors(const TArray<ANiagaraActor*>& InNiagaraActors);

    /** エフェクトタイプの設定 */
    void SetEffectType(EBuffEffect NewEffect);

    // =======================
    // 色の適用
    // =======================

    /** マテリアルに色を適用 */
    void ApplyColorToMaterial(const FLinearColor& InColor);

    /** 現在の色を取得 */
    FORCEINLINE FLinearColor GetCurrentColor() const { return CurrentColor; }

    // =======================
    // 色判定ロジック（UColorUtilityLibraryへの委譲）
    // =======================

    //別クラスに全て委託

    // =======================
    // 色変換（UColorUtilityLibraryへの委譲）
    // =======================

    /** 補色を計算 */
    FORCEINLINE FLinearColor GetComplementaryColor(const FLinearColor& InColor) const
    {
        return UColorUtilityLibrary::GetComplementaryColor(InColor);
    }

    // =======================
    // 状態管理
    // =======================

    /** 選択モードの設定 */
    void SetSelectMode(bool bIsNowSelected);

    /** 非表示状態の取得 */
    FORCEINLINE bool IsHidden() const { return bHide; }

    /** 非表示状態の設定 */
    void SetHidden(bool bInHide);

    // =======================
    // エフェクト制御
    // =======================

    /** Niagaraの表示/非表示を切り替え */
    void ToggleNiagaraActiveState(bool bVisible);

    /** 出現エフェクトを再生 */
    void PlayAppearEffect();

    /** 全エフェクトを無効化 */
    void DeactivateAllEffects();
   /** 色がマッチした時のコールバック */
    virtual bool OnColorMatched(const FLinearColor& FilterColor);

    /** 色がミスマッチした時のコールバック */
    virtual bool OnColorMismatched(const FLinearColor& FilterColor);
protected:
    // =======================
    // 内部処理（派生クラスで拡張可能）
    // =======================

 

    /** Niagaraエフェクトを起動 */
    void ActivateNiagaraEffect(UNiagaraSystem* NiagaraSystem);

    // =======================
    // プロパティ
    // =======================

    /** ダイナミックマテリアル */
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynMesh;

    /** 現在の色 */
    UPROPERTY()
    FLinearColor CurrentColor;

    /** エフェクトタイプ */
    UPROPERTY()
    EBuffEffect Effect;

    /** 選択状態 */
    UPROPERTY()
    bool bSelected;

    /** オブジェクト自体は非表示にならないため、boolで管理 */
    UPROPERTY()
    bool bHide;

    /** Niagaraアクター配列 */
    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<ANiagaraActor>> Niagaras;

    /** アクティブなNiagaraコンポーネント */
    UPROPERTY()
    TArray<TObjectPtr<UNiagaraComponent>> ActiveNiagaraComponents;

    // =======================
    // Niagaraシステムアセット
    // =======================

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> FireflyBurstNiagaraSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> ParticlesOfLightNiagaraSystem;

    UPROPERTY()
    TObjectPtr<UNiagaraSystem> LightCubeNiagaraSystem;
};