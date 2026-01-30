// GrowingTreeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "GrowingTreeComponent.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

/**
 * @brief 木の成長段階
 */
UENUM(BlueprintType)
enum class ETreeGrowthStage : uint8
{
    Sapling UMETA(DisplayName = "苗"),
    WaitingForWater UMETA(DisplayName = "水待ち"),
    Growing UMETA(DisplayName = "成長中"),
    FullyGrown UMETA(DisplayName = "成木")
};

/**
 * @brief 成長モード
 */
UENUM(BlueprintType)
enum class EGrowthMode : uint8
{
    MeshSwap UMETA(DisplayName = "メッシュ切り替え"),
    Animation UMETA(DisplayName = "アニメーション再生")
};

/**
 * @brief 成長トリガーモード
 */
UENUM(BlueprintType)
enum class EGrowthTriggerMode : uint8
{
    TwoStep UMETA(DisplayName = "2段階（緑→青）"),
    GreenOnly UMETA(DisplayName = "緑のみ")
};

/**
 * @brief 成長する木のコンポーネント
 *
 * 使い方:
 * - 2段階モード: 緑色(Green)で水分受付→青色(Blue)で成長
 * - 緑のみモード: 緑色(Green)で即座に成長開始
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UGrowingTreeComponent : public UObjectColorComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ
     */
    UGrowingTreeComponent();

protected:
    /**
     * @brief 実行開始時の初期化処理
     */
    virtual void BeginPlay() override;

    /**
     * @brief フレーム毎の更新処理
     */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * @brief 初期化処理
     * 木の初期状態やメッシュ設定を行う
     */
    virtual void Initialize() override;

    /**
     * @brief 色受信処理
     * モードに応じて成長トリガーを処理
     * @param InColor 新たに適用された色
     */
    virtual void ActivateDirect(const FLinearColor& InColor) override;

    /**
     * @brief 成長段階を設定
     * @param NewStage 新しい成長段階
     */
    UFUNCTION(BlueprintCallable, Category = "Tree")
    void SetGrowthStage(ETreeGrowthStage NewStage);

    /**
     * @brief 現在の成長段階を取得
     */
    UFUNCTION(BlueprintPure, Category = "Tree")
    ETreeGrowthStage GetGrowthStage() const { return CurrentStage; }

    /**
     * @brief 水分受付可能状態かどうか
     */
    UFUNCTION(BlueprintPure, Category = "Tree")
    bool CanReceiveWater() const { return CurrentStage == ETreeGrowthStage::WaitingForWater; }

protected:
    /**
     * @brief 水分受付可能状態にする（緑色を受けた時・2段階モード用）
     */
    void ActivateWaterReady();

    /**
     * @brief 成長を開始する
     */
    void StartGrowth();

    /**
     * @brief 成長アニメーションの更新
     * @param DeltaTime フレーム時間
     */
    void UpdateGrowth(float DeltaTime);

    /**
     * @brief 成長を完了させる
     */
    void CompleteGrowth();

    /**
     * @brief 成長段階に応じたメッシュを設定（メッシュ切り替えモード用）
     * @param Stage 成長段階
     */
    void SetMeshForStage(ETreeGrowthStage Stage);

    /**
     * @brief アニメーションを再生（アニメーションモード用）
     */
    void PlayGrowthAnimation();

    /**
     * @brief スケールアニメーションを更新（アニメーションモード用）
     * @param DeltaTime フレーム時間
     */
    void UpdateScaleAnimation(float DeltaTime);

    /**
     * @brief 成長エフェクトを再生
     */
    void PlayGrowthEffect();

    /**
     * @brief 水エフェクトを再生
     */
    void PlayWaterEffect();

    /**
     * @brief メッシュコンポーネントを取得
     */
    UStaticMeshComponent* GetTreeMeshComponent() const;

    /**
     * @brief スケルタルメッシュコンポーネントを取得（アニメーションモード用）
     */
    USkeletalMeshComponent* GetSkeletalMeshComponent() const;

protected:
    // =======================
    // 基本設定
    // =======================

    /** 成長トリガーモード */
    UPROPERTY(EditAnywhere, Category = "Tree|Mode")
    EGrowthTriggerMode TriggerMode = EGrowthTriggerMode::TwoStep;

    /** 成長モード */
    UPROPERTY(EditAnywhere, Category = "Tree|Mode")
    EGrowthMode GrowthMode = EGrowthMode::MeshSwap;

    // =======================
    // メッシュ切り替えモード用
    // =======================

    /** 苗のメッシュ */
    UPROPERTY(EditAnywhere, Category = "Tree|MeshSwap", meta = (EditCondition = "GrowthMode == EGrowthMode::MeshSwap", EditConditionHides))
    TObjectPtr<UStaticMesh> SaplingMesh;

    /** 苗のメッシュスケール */
    UPROPERTY(EditAnywhere, Category = "Tree|MeshSwap", meta = (EditCondition = "GrowthMode == EGrowthMode::MeshSwap", EditConditionHides))
    FVector SaplingMeshScale = FVector::OneVector;

    /** 成木のメッシュ */
    UPROPERTY(EditAnywhere, Category = "Tree|MeshSwap", meta = (EditCondition = "GrowthMode == EGrowthMode::MeshSwap", EditConditionHides))
    TObjectPtr<UStaticMesh> FullyGrownMesh;

    /** 成木のメッシュスケール */
    UPROPERTY(EditAnywhere, Category = "Tree|MeshSwap", meta = (EditCondition = "GrowthMode == EGrowthMode::MeshSwap", EditConditionHides))
    FVector FullyGrownMeshScale = FVector::OneVector;

    // =======================
    // アニメーションモード用
    // =======================

    /** 成長アニメーション */
    UPROPERTY(EditAnywhere, Category = "Tree|Animation", meta = (EditCondition = "GrowthMode == EGrowthMode::Animation", EditConditionHides))
    TObjectPtr<UAnimSequence> GrowthAnimation;

    /** アニメーション再生速度 */
    UPROPERTY(EditAnywhere, Category = "Tree|Animation", meta = (EditCondition = "GrowthMode == EGrowthMode::Animation", EditConditionHides, ClampMin = "0.1"))
    float AnimationPlayRate = 1.0f;

    /** スケール変化を有効化 */
    UPROPERTY(EditAnywhere, Category = "Tree|Animation", meta = (EditCondition = "GrowthMode == EGrowthMode::Animation", EditConditionHides))
    bool bEnableScaleChange = true;

    /** 成長開始時のスケール倍率 */
    UPROPERTY(EditAnywhere, Category = "Tree|Animation", meta = (EditCondition = "GrowthMode == EGrowthMode::Animation && bEnableScaleChange", EditConditionHides, ClampMin = "0.1"))
    float StartScaleMultiplier = 0.5f;

    /** 成長完了時のスケール倍率 */
    UPROPERTY(EditAnywhere, Category = "Tree|Animation", meta = (EditCondition = "GrowthMode == EGrowthMode::Animation && bEnableScaleChange", EditConditionHides, ClampMin = "0.1"))
    float EndScaleMultiplier = 2.0f;

    // =======================
    // 共通パラメータ
    // =======================

    /** 成長にかかる時間（秒） */
    UPROPERTY(EditAnywhere, Category = "Tree|Growth", meta = (ClampMin = "0.1"))
    float GrowthDuration = 1.5f;

    /** 成長カーブ（オプション） */
    UPROPERTY(EditAnywhere, Category = "Tree|Growth")
    TObjectPtr<UCurveFloat> GrowthCurve;

    /** 水分補給状態の持続時間（秒）0で無限 */
    UPROPERTY(EditAnywhere, Category = "Tree|Growth", meta = (EditCondition = "TriggerMode == EGrowthTriggerMode::TwoStep", EditConditionHides, ClampMin = "0.0"))
    float WaterReadyDuration = 5.0f;

    // =======================
    // エフェクト
    // =======================

    /** 水エフェクト（2段階モード用） */
    UPROPERTY(EditAnywhere, Category = "Tree|Effects", meta = (EditCondition = "TriggerMode == EGrowthTriggerMode::TwoStep", EditConditionHides))
    TObjectPtr<UNiagaraSystem> WaterEffect;

    /** 成長エフェクト */
    UPROPERTY(EditAnywhere, Category = "Tree|Effects")
    TObjectPtr<UNiagaraSystem> GrowthEffect;

    /** 成長サウンド */
    UPROPERTY(EditAnywhere, Category = "Tree|Effects")
    TObjectPtr<USoundBase> GrowthSound;

    /** 水分サウンド（2段階モード用） */
    UPROPERTY(EditAnywhere, Category = "Tree|Effects", meta = (EditCondition = "TriggerMode == EGrowthTriggerMode::TwoStep", EditConditionHides))
    TObjectPtr<USoundBase> WaterSound;

    // =======================
    // 色設定
    // =======================

    /** 成長開始カラー（緑） */
    UPROPERTY(EditAnywhere, Category = "Tree|Colors")
    FLinearColor GrowthTriggerColor = FLinearColor::Green;

    /** 2段階目トリガーカラー（青・2段階モード用） */
    UPROPERTY(EditAnywhere, Category = "Tree|Colors", meta = (EditCondition = "TriggerMode == EGrowthTriggerMode::TwoStep", EditConditionHides))
    FLinearColor SecondStepColor = FLinearColor::Blue;

    /** 色判定の許容誤差 */
    UPROPERTY(EditAnywhere, Category = "Tree|Colors", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ColorTolerance = 0.1f;

private:
    // =======================
    // 状態管理
    // =======================

    /** 現在の成長段階 */
    UPROPERTY(VisibleAnywhere, Category = "Tree|State")
    ETreeGrowthStage CurrentStage;

    /** 成長中フラグ */
    bool bIsGrowing;

    /** 成長タイマー */
    float GrowthTimer;

    /** 水分補給タイマー */
    float WaterReadyTimer;

    /** 成長開始時のスケール */
    FVector InitialScale;

    /** 成長完了時のスケール */
    FVector TargetScale;

    /** 元のスケール（アニメーションモード用） */
    FVector OriginalScale;
};