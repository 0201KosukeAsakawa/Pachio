#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "DataContainer/ColorTargetTypes.h"
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
    /**
     * コンストラクタ。デフォルトの値を設定する
     */
    AColorReactiveObject();

protected:
    /**
     * ゲーム開始時またはスポーン時に呼ばれる初期化処理
     */
    virtual void BeginPlay() override;

    /**
     * オブジェクト固有の初期化処理（派生クラスでオーバーライド可能）
     */
    virtual void Initialize();

public:
    // =======================
    // IColorReactiveInterface の実装
    // =======================

    /**
     * 指定された色を適用する
     *
     * @param NewColor 適用する色
     */
    virtual void ApplyColorWithMatching(const FLinearColor& NewColor) override;

    /**
     * 色を初期状態にリセットする
     */
    virtual void ResetColor() override;

    /**
     * オブジェクトの選択状態を設定する
     *
     * @param bIsSelected 選択中なら true
     */
    virtual void SetSelected(bool bIsSelected) override;

    /**
     * 現在の色が変更されているかを確認する
     *
     * @return 変更されていれば true
     */
    virtual bool HasColorChanged() const override;

    /**
     * 色の変更が可能かを確認する
     *
     * @return 変更可能であれば true
     */
    virtual bool IsChangeable() const override;

    /**
     * 現在の色が基準色と一致しているかを確認する
     *
     * @return 一致していれば true
     */
    virtual bool IsColorMatched() const override;

    /**
     * 色イベントの識別用IDを取得する
     *
     * @return 色イベントID
     */
    virtual FName GetColorEventID() const override;

    // =======================
    // 状態管理
    // =======================

    /**
     * 現在の色を取得する
     *
     * @return 現在の色
     */
    FLinearColor GetCurrentColor() const;

    /**
     * 初期設定時の色を取得する
     *
     * @return 初期色
     */
    FLinearColor GetInitialColor() const;

protected:
    // =======================
    // 色処理の委譲メソッド
    // =======================

    /**
     * 色のマッチング処理を実行する
     *
     * @param NewColor 判定対象の色
     */
    void ProcessColorMatching(const FLinearColor& NewColor);

    // =======================
    // コンポーネント
    // =======================

    /** オブジェクトの色状態を管理するコンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UObjectColorComponent> ObjectColorComponent;

    // =======================
    // 設定
    // =======================

    /** ビートアニメーションを有効化するかどうか */
    UPROPERTY(EditAnywhere, Category = "Effects")
    bool bEnableBeatAnimation;
};
