// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "ColorReactiveComponent.generated.h"


class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveComponent : public UObjectColorComponent
{
    GENERATED_BODY()

public:
    /**
     * コンストラクタ（デフォルト値を設定）
     */
    UColorReactiveComponent();
    // =======================
    // 状態管理
    // =======================

    /**
     * 非表示状態を取得
     *
     * @return 非表示であれば true
     */
    FORCEINLINE bool IsHidden() const { return bHide; }


private:
    /**
     * 指定された色を適用し、必要に応じてマッチング処理を行う
     *
     * @param NewColor - 適用する新しい色
     */
    virtual void ApplyColorWithMatching(const FLinearColor& NewColor)override;
    //// =======================
    //// エフェクト制御
    //// =======================

    /**
     * 色がマッチした時に呼ばれる
     *
     * @param FilterColor - 判定に使用された色
     * @return 成功した場合は true
     */
    virtual bool OnColorMatched(const FLinearColor& FilterColor) { return true; };

    /**
     * 色がミスマッチした時に呼ばれる
     *
     * @param FilterColor - 判定に使用された色
     * @return 処理した場合は true
     */
    virtual bool OnColorMismatched(const FLinearColor& FilterColor) { return true; };
protected:
    // =======================
    // プロパティ
    // =======================

    /**
     * 非表示状態（オブジェクト自体は非表示にならないためフラグで管理）
     */
    UPROPERTY()
    bool bHide;

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
