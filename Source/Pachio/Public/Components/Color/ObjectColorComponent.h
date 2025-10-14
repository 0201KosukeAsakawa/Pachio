#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorUtilityLibrary.h"
#include "DataContainer/EffectMatchResult.h"
#include "ObjectColorComponent.generated.h"

class ALevelManager;
class ANiagaraActor;
class UBeatScalerComponent;
class UColorReactiveComponent;
class UColorManager;



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UObjectColorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UObjectColorComponent();

    // =======================
    // 初期化
    // =======================

    /** コンポーネントの初期化を実行 */
    void Initialize();

    // =======================
    // 色の操作
    // =======================

    /** 新しい色を設定し、マテリアルとエフェクトに反映 */
    void SetColor(const FLinearColor& NewColor, const FEffectMatchResult& MatchResult);

    /** 色を初期状態にリセット */
    void ResetColor(const FEffectMatchResult& MatchResult);

    /** 内部的に現在の色のみを更新（マテリアルには反映しない） */
    void SetCurrentColorOnly(const FLinearColor& NewColor);

    /** 色のマッチング処理を実行 */
    void ProcessColorMatching(const FLinearColor& NewColor, const FEffectMatchResult& MatchResult);

    // =======================
    // 状態の取得と設定
    // =======================

    /** 色が一致しているかを取得 */
    FORCEINLINE bool IsColorMatched() const { return bColorMatched; }

    /** 色の一致状態を設定 */
    void SetColorMatched(bool bMatched);

    /** 選択状態を設定 */
    void SetSelected(bool bSelected);

    /** 色変更が可能かを取得 */
    FORCEINLINE bool IsChangeable() const { return bColorChangeable; }

    /** 非表示状態かを取得 */
    bool IsHidden() const;

    /** 現在の色を取得 */
    FORCEINLINE FLinearColor GetCurrentColor() const { return CurrentColor; }

    /** 初期色を取得 */
    FORCEINLINE FLinearColor GetInitialColor() const { return InitialColor; }

    /** 色イベントIDを取得 */
    FORCEINLINE FName GetColorEventID() const { return ColorEventID; }

    // =======================
    // 色の判定
    // =======================

    /** 初期色から変更されているかを判定 */
    bool HasColorChanged(const  float Tolerance = 30.f) const;

    /** 指定色と現在の色が一致しているかを判定 */
    bool HasColorChanged(const FLinearColor& CompareColor, float Tolerance = 30.f) const;

    // =======================
    // エフェクト処理
    // =======================

    /** マテリアルに色を適用 */
    void ApplyColorToMaterial(const FLinearColor& Color);
protected:
    // =======================
    // 内部初期化処理
    // =======================

    /** 色ロジックの初期化 */
    void InitializeColorLogic();

    /** カラーマネージャーへの登録 */
    void RegisterToColorManager();

    /** マテリアルの初期設定 */
    void SetupMaterial();

    // =======================
    // ヘルパー関数
    // =======================

    /** SkeletalMeshComponentを取得 */
    USkeletalMeshComponent* GetMeshComponent() const;

    /** レベルマネージャーを取得 */
    ALevelManager* GetLevelManager() const;

    /** カラーマネージャーを取得 */
    UColorManager* GetColorManager() const;

    // =======================
    // コンポーネントとプロパティ
    // =======================
protected:
    /** 色リアクティブコンポーネント */
    UPROPERTY()
    TObjectPtr<UColorReactiveComponent> ColorReactive;

    /** 色リアクティブコンポーネントのクラス */
    UPROPERTY(EditAnywhere, Category = "Color|Setup")
    TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;

    // =======================
    // 色の状態
    // =======================

    /** 現在の色 */
    UPROPERTY(VisibleAnywhere, Category = "Color|State")
    FLinearColor CurrentColor;

    /** 初期色 */
    FLinearColor InitialColor;

    // =======================
    // エフェクト設定
    // =======================

    /** エフェクトタイプ */
    UPROPERTY(EditAnywhere, Category = "Effects")
    EBuffEffect EffectType;

    /** Niagaraアクター配列 */
    UPROPERTY(EditAnywhere, Category = "Effects")
    TArray<TObjectPtr<ANiagaraActor>> NiagaraActors;

    // =======================
    // 動作フラグ
    // =======================

    /** マテリアルへの色適用を有効化 */
    UPROPERTY(EditAnywhere, Category = "Color|Behavior")
    bool bApplyColorToMaterial;

    /** 色変更アクションを有効化 */
    UPROPERTY(EditAnywhere, Category = "Color|Behavior")
    bool bEnableColorAction;

    /** ビート演出を有効化 */
    UPROPERTY(EditAnywhere, Category = "Effects|Behavior")
    bool bEnableBeatEffect;
    /** 補色を使用する */
    UPROPERTY(EditAnywhere, Category = "Color|Matching")
    bool bUseComplementaryColor;

    // =======================
    // 登録設定
    // =======================

    /** カラーマネージャーへの登録ターゲットタイプ */
    UPROPERTY(EditAnywhere, Category = "Color|Registration")
    EColorTargetType TargetType;

    /** 色イベントID（イベントトリガー用） */
    UPROPERTY(EditAnywhere, Category = "Color|Events")
    FName ColorEventID;

private:
    // =======================
    // 内部状態
    // =======================

    /** 色が一致しているか */
    bool bColorMatched;

    /** 選択されているか */
    bool bSelected;

    /** 色変更が可能か */
    UPROPERTY(EditAnywhere, Category = "Color|State")
    bool bColorChangeable;

    // =======================
    // 定数
    // =======================

    /** カスタムデプスステンシル値 */
    static constexpr int32 CUSTOM_DEPTH_STENCIL_VALUE = 10;

    /** マテリアルスロットインデックス */
    static constexpr int32 MATERIAL_SLOT_INDEX = 0;
};