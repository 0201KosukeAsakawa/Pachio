#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveObject.generated.h"

class UColorReactiveComponent;
class UBeatScalerComponent;
class UObjectColorComponent;

/**
 * 色に反応するオブジェクトの基底クラス
 * UObjectColorComponentを使用して色管理を行う
 */
UCLASS()
class PACHIO_API AColorReactiveObject : public AActor, public IColorReactiveInterface
{
    GENERATED_BODY()

public:
    AColorReactiveObject();
    
protected:
    virtual void BeginPlay() override;

    /** オブジェクト固有の初期化処理（派生クラスでオーバーライド可能） */
    virtual void Initialize();

public:
    // =======================
    // IColorReactiveInterface の実装
    // =======================

    /** 色を設定 */
    virtual void ApplyColorWithMatching(const FLinearColor& NewColor, const FEffectMatchResult& MatchResult) override;

    /** 色をリセット */
    virtual void ResetColor(const FEffectMatchResult& MatchResult) override;

    /** 選択状態を設定 */
    virtual void SetSelected(bool bIsSelected) override;

    /** 色が変更されているかを確認 */
    virtual bool HasColorChanged() const override;

    /** 色変更が可能かを確認 */
    virtual bool IsChangeable() const override;

    /** 色が一致しているかを確認 */
    virtual bool IsColorMatched() const override;

    /** 色イベントIDを取得 */
    virtual FName GetColorEventID() const override;

    // =======================
    // エフェクトとアニメーション
    // =======================

    // =======================
    // 状態管理
    // =======================
    /** 現在の色を取得 */
    FLinearColor GetCurrentColor() const;

    /** 初期色を取得 */
    FLinearColor GetInitialColor() const;

protected:
    // =======================
    // 色処理の委譲メソッド
    // =======================

    /** 色のマッチング処理を実行 */
    void ProcessColorMatching(const FLinearColor& NewColor, const FEffectMatchResult& MatchResult);

    // =======================
    // コンポーネント
    // =======================

    /** オブジェクトの色管理コンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UObjectColorComponent> ObjectColorComponent;

    // =======================
    // 設定
    // =======================

    /** ビートアニメーションを有効化 */
    UPROPERTY(EditAnywhere, Category = "Effects")
    bool bEnableBeatAnimation;
};