// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OutlineComponent.generated.h"


// ============================================================================
// UOutlineComponent
// アクターにアウトライン効果を付与するためのコンポーネント
// ============================================================================

/**
 * @brief アクターにアウトライン表示を付与するコンポーネント
 *
 * 本コンポーネントをアクターに追加することで、
 * 指定されたマテリアルを用いたアウトライン表示を行うことができる。
 * Tick により動的な制御（ON/OFF やフェード処理など）にも対応可能。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UOutlineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief デフォルトコンストラクタ
     *
     * コンポーネントの初期化を行う。
     * Tick の有効化設定など、基本的なセットアップを実施。
     */
    UOutlineComponent();

protected:
    /**
     * @brief コンポーネント開始時に呼ばれる処理
     *
     * ゲーム開始時またはアクター生成時に呼び出され、
     * アウトラインメッシュの初期設定やマテリアル適用処理を行う。
     */
    virtual void BeginPlay() override;

    /**
     * @brief 毎フレーム実行される処理
     *
     * アウトラインの点滅や強調表示の更新など、
     * 毎フレームごとに変化するアウトライン演出を制御する。
     *
     * @param DeltaTime 経過時間
     * @param TickType ティックの種類
     * @param ThisTickFunction Tick 関数の詳細構造体
     */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

    /**
     * @brief アウトライン描画用メッシュへの参照
     *
     * 対象アクターのメッシュとは別に、アウトライン用に設定されたメッシュ。
     * 通常、複製またはアタッチによって表示される。
     */
    UPROPERTY()
    UStaticMeshComponent* OutlineMesh;

    /**
     * @brief アウトライン描画に使用するマテリアル
     *
     * アウトライン表示専用のマテリアル。エディタ上で設定可能。
     */
    UPROPERTY(EditDefaultsOnly, Category = "Outline")
    UMaterialInterface* OutlineMaterial;
};
