#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorUtilityLibrary.h"
#include "Interface/ColorFilterInterface.h"
#include "ObjectColorComponent.generated.h"

class ALevelManager;
class ANiagaraActor;
class UBeatScalerComponent;
class UColorReactiveComponent;
class UColorManager;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnColorCategoryChanged, EColorCategory);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UObjectColorComponent : public USceneComponent, public IColorReactiveInterface
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ
     *
     * コンポーネント生成時にデフォルト値を初期化します。
     * Unreal Engine のライフサイクル的に、まだアタッチ先アクターは確定していません。
     */
    UObjectColorComponent();

    /**
     * @brief 実行開始時の初期化処理
     *
     * ゲーム開始時（またはアクターがスポーンした時）に呼び出されます。
     * コンポーネントの初期化、マネージャー登録、マテリアル設定などを行うのに適しています。
     */
    virtual void BeginPlay() override;

    void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction)override;

#if WITH_EDITOR
    /**
     * @brief プロパティ変更時のコールバック（エディタ専用）
     *
     * エディタ上でプロパティを変更した際に呼ばれます。
     * 値変更に応じてリアルタイムで初期化・反映を行いたい場合に使用します。
     *
     * @param PropertyChangedEvent 変更されたプロパティに関する情報
     */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // =======================
    // 初期化
    // =======================

    /**
     * コンポーネント全体の初期化
     * 各種マネージャーへの登録とイベントバインドを行う
     */
    virtual void Initialize();

    /**
     * 指定された色を適用し、必要に応じてマッチング処理を行う
     *
     * @param NewColor - 適用する新しい色
     */
    virtual void ApplyColorWithMatching(const FLinearColor& NewColor)override;

    // =======================
    // 色の操作
    // =======================

    /**
     * 色を設定し、マテリアルとエフェクトに反映
     *
     * @param NewColor 新しい色
     */
    void SetColor(const FLinearColor& NewColor);

    /**
     * 色を初期状態にリセット
     *
     * @param MatchResult エフェクトマッチング結果
     */
    void ResetColor();

    /**
     * 内部的に現在の色のみを更新
     * マテリアルやエフェクトには反映しない（軽量な更新用）
     *
     * @param NewColor 新しい色
     */
    void SetCurrentColorOnly(const FLinearColor& NewColor);

    // =======================
    // 状態の取得と設定
    // =======================

    /**
     * 現在の色がマッチしているかを判定する
     *
     * @return マッチしていれば true
     */
    FORCEINLINE bool IsColorMatched() const final override { return bColorMatched; }

    /**
     * 色の一致状態を設定
     *
     * @param bMatched 一致しているか
     */
    void SetColorMatched(bool bMatched);

    /**
    * 色が変更可能かを判定する
    *
    * @return 変更可能であれば true
    */
    FORCEINLINE bool IsChangeable() const final override { return bColorChangeable; }

    /** 現在の色を取得 */
    FORCEINLINE FLinearColor GetCurrentColor() const { return CurrentColor; }

    /** 初期色を取得 */
    FORCEINLINE FLinearColor GetInitialColor() const { return InitialColor; }

    /**
     * カラーイベントを識別するための ID を取得する
     *
     * @return カラーイベントの識別子（FName）
     */
    FORCEINLINE FName GetColorEventID() const final override { return ColorEventID; }

    // =======================
    // 色の判定
    // =======================

    /**
     * 初期色から変更されているかを判定
     *
     * @return 変更されている場合true
     */
    bool HasColorChanged(const  float Tolerance = 30.f) const;

    // =======================
    // エフェクト処理
    // =======================

    /**
     * マテリアルに色を適用
     * ColorReactiveを通じて実際のマテリアル更新を行う
     *
     * @param Color 適用する色
     */
    void ApplyColorToMaterial(const FLinearColor& Color);
    void ApplyColorToMaterialAlpha(const float Alpha, const FLinearColor& InColor);
protected:
    // =======================
    // 内部初期化処理
    // =======================

    /**
     * 色ロジックの初期化
     * ColorReactiveComponentを生成し、初期色を設定する
     */
    void InitializeColorLogic();

    /**
     * カラーマネージャーへの登録
     * このオブジェクトを色管理対象として登録する
     */
    void RegisterToColorManager();

    /**
     * マテリアルの初期設定
     * カスタムデプスレンダリングとダイナミックマテリアルを設定
     */
    void SetupMaterial();

    // =======================
    // ヘルパー関数
    // =======================

    /**
    * SkeletalMeshComponentを取得
    * オーナーアクターから"Mesh"という名前のコンポーネントを検索
    *
    * @return SkeletalMeshComponent（見つからない場合はnullptr）
    */
    UStaticMeshComponent* GetMeshComponent() const;

    /**
     * レベルマネージャーを取得
     *
     * @return ALevelManager（見つからない場合はnullptr）
     */
    ALevelManager* GetLevelManager() const;

    /**
     * カラーマネージャーを取得
     * レベルマネージャーを経由して取得
     *
     * @return UColorManager（見つからない場合はnullptr）
     */
    UColorManager* GetColorManager() const;


    void UpdateHitColorTransition(FLinearColor);

    // =======================
    // コンポーネントとプロパティ
    // =======================
public:
    // グローバルやクラスメンバとしてデリゲートを宣言
    FOnColorCategoryChanged OnColorChanged;

protected:
    // =======================
    // 色の状態
    // =======================

    /** 現在の色 */
    UPROPERTY(VisibleAnywhere, Category = "Color|State")
    FLinearColor CurrentColor;

    FLinearColor HitColor;
    /** 初期色 */
    FLinearColor InitialColor;

    // =======================
    // エフェクト設定
    // =======================

    /** エフェクトタイプ */
    UPROPERTY(EditAnywhere, Category = "Category")
    EColorCategory ColorCategory;

    /**
     * ダイナミックマテリアル
     */
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynMesh;

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

    /** 初期化済みであるか */
    bool bInitialized;

    float HitTimer;

    float LastPaintTime = 0;

    FLinearColor StartColor;
    FLinearColor LastColor;

    bool bIsPlayedPaint = false;
    bool bIsPainting = false;
};