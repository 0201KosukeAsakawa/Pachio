// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorUtilityLibrary.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorReactiveComponent.generated.h"


class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /**
     * コンストラクタ（デフォルト値を設定）
     */
    UColorReactiveComponent();

    // =======================
    // 初期化
    // =======================

    /**
     * 色とマテリアルの初期化
     *
     * @param InitialColor - 初期色
     * @param bVariable - 色を変数として扱うか（動的変更可能）
     * @param Owner - オーナーアクター（nullptr の場合は GetOwner() を使用）
     */
    virtual void Initialize(const FLinearColor& InitialColor, bool bVariable, AActor* Owner = nullptr);

    /**
     * Niagaraアクターの設定
     *
     * @param InNiagaraActors - 使用する Niagara アクター配列
     */
    void SetupNiagaraActors(const TArray<ANiagaraActor*>& InNiagaraActors);

    /**
     * エフェクトタイプの設定
     *
     * @param NewEffect - 設定するエフェクトタイプ
     */
    void SetEffectType(EColorCategory NewEffect);

    // =======================
    // 色の適用
    // =======================

    /**
     * マテリアルに指定された色を適用
     *
     * @param InColor - 適用する色
     */
    void ApplyColorToMaterial(const FLinearColor& InColor);

    /**
     * 現在の色を取得
     *
     * @return 現在設定されている色
     */
    FORCEINLINE FLinearColor GetCurrentColor() const { return CurrentColor; }

    // =======================
    // 色判定ロジック（UColorUtilityLibraryへの委譲）
    // =======================

    // ※別クラス（UColorUtilityLibrary）にすべて委譲

    // =======================
    // 色変換（UColorUtilityLibraryへの委譲）
    // =======================

    /**
     * 指定された色の補色を取得
     *
     * @param InColor - 元の色
     * @return 補色
     */
    FORCEINLINE FLinearColor GetComplementaryColor(const FLinearColor& InColor) const
    {
        return UColorUtilityLibrary::GetComplementaryColor(InColor);
    }

    // =======================
    // 状態管理
    // =======================

    /**
     * 選択モードを設定
     *
     * @param bIsNowSelected - 選択状態にするか
     */
    void SetSelectMode(bool bIsNowSelected);

    /**
     * 非表示状態を取得
     *
     * @return 非表示であれば true
     */
    FORCEINLINE bool IsHidden() const { return bHide; }

    /**
     * 非表示状態を設定
     *
     * @param bInHide - 非表示状態に設定するか
     */
    void SetHidden(bool bInHide);

    // =======================
    // エフェクト制御
    // =======================

    /**
     * Niagaraの表示／非表示を切り替え
     *
     * @param bVisible - 表示する場合は true
     */
    void ToggleNiagaraActiveState(bool bVisible);

    /**
     * 出現時エフェクトを再生
     */
    void PlayAppearEffect();

    /**
     * すべてのエフェクトを無効化
     */
    void DeactivateAllEffects();

    /**
     * 色がマッチした時に呼ばれるコールバック
     *
     * @param FilterColor - 判定に使用された色
     * @return 成功した場合は true
     */
    virtual bool OnColorMatched(const FLinearColor& FilterColor);

    /**
     * 色がミスマッチした時に呼ばれるコールバック
     *
     * @param FilterColor - 判定に使用された色
     * @return 処理した場合は true
     */
    virtual bool OnColorMismatched(const FLinearColor& FilterColor);

protected:
    // =======================
    // 内部処理（派生クラスで拡張可能）
    // =======================

    /**
     * 指定された Niagara システムを起動
     *
     * @param NiagaraSystem - 起動する Niagara システム
     */
    void ActivateNiagaraEffect(UNiagaraSystem* NiagaraSystem);

    // =======================
    // プロパティ
    // =======================

    /**
     * ダイナミックマテリアル
     */
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynMesh;

    /**
     * 現在の色
     */
    UPROPERTY()
    FLinearColor CurrentColor;

    /**
     * エフェクトタイプ
     */
    UPROPERTY()
    EColorCategory Effect;

    /**
     * 選択状態
     */
    UPROPERTY()
    bool bSelected;

    /**
     * 非表示状態（オブジェクト自体は非表示にならないためフラグで管理）
     */
    UPROPERTY()
    bool bHide;

    /**
     * 使用する Niagara アクター配列
     */
    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<ANiagaraActor>> Niagaras;

    /**
     * 現在アクティブな Niagara コンポーネント一覧
     */
    UPROPERTY()
    TArray<TObjectPtr<UNiagaraComponent>> ActiveNiagaraComponents;

    // =======================
    // Niagaraシステムアセット
    // =======================

    /** ホタル風の発光エフェクト */
    UPROPERTY()
    TObjectPtr<UNiagaraSystem> FireflyBurstNiagaraSystem;

    /** 光の粒子エフェクト */
    UPROPERTY()
    TObjectPtr<UNiagaraSystem> ParticlesOfLightNiagaraSystem;

    /** 光の立方体エフェクト */
    UPROPERTY()
    TObjectPtr<UNiagaraSystem> LightCubeNiagaraSystem;
};
