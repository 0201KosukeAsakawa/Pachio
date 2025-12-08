// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorTargetRegistry.generated.h"

class IColorReactiveInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorAppliedDelegate, FLinearColor, NewColor);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorTargetRegistry : public UActorComponent
{
    GENERATED_BODY()

public:
    /**
     * 指定したモードに応じて対象へ色を適用する
     *
     * @param NewColor - 適用する色
     * @param Mode - 色を適用する対象モード
     */
    void ApplyColor(FLinearColor NewColor);

    /**
     * 指定されたイベント ID に基づいて色イベントを発火する
     *
     * @param EventID - イベント識別用の名前
     * @param Color - 対象となる色
     */
    void ColorEvent(FName EventID, FLinearColor Color);

    /**
     * 色付け対象を登録する
     *
     * @param Target - 登録するインターフェース
     */
    void RegisterTarget(TScriptInterface<IColorReactiveInterface> Target);

    /**
     * ポストエフェクト用マテリアルを初期化する
     */
    void InitializePostEffect();

    /**
     * 現在のポストプロセスカラーを取得する
     *
     * @return ポストプロセスカラー
     */
    FLinearColor GetPostProcessColor() const;

    /** 色適用時に呼ばれるデリゲート（Blueprintで購読可能） */
    UPROPERTY(BlueprintAssignable, Category = "Color")
    FOnColorAppliedDelegate OnColorApplied;

private:
    /**
     * 指定モードに登録された全ターゲットへ色変更を通知する
     *
     * @param Mode - 対象モード
     * @param Color - 通知する色
     */
    void NotifyTargets(const FLinearColor& Color);

private:
    /**
     * 各色モードに対応する反応ターゲット群
     * （EColorTargetType ごとに IColorReactiveInterface 実装オブジェクトを保持）
     */
    UPROPERTY()
    TArray<TScriptInterface<IColorReactiveInterface>> Instances;

    /**
     * ポストプロセスマテリアルの動的インスタンス
     * （実行時に色を変更可能なマテリアル）
     */
    UPROPERTY()
    UMaterialInstanceDynamic* PostProcessMID;

private:
    /**
     * ポストプロセスマテリアル（エディタ上で設定可能）
     */
    UPROPERTY(EditAnywhere)
    UMaterialInterface* PostProcessMaterial;
};
