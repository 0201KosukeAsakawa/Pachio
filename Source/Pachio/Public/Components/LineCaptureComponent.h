// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneCaptureComponent2D.h" // SceneCaptureComponent2D をインクルード
#include "Engine/TextureRenderTarget2D.h" // UTextureRenderTarget2D をインクルード
#include "Materials/MaterialInterface.h" // UMaterialInterface をインクルード
#include "LineCaptureComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API ULineCaptureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    ULineCaptureComponent();

protected:
    virtual void BeginPlay() override;
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    /**
     * @brief アウトラインを描画する対象オブジェクトに設定するタグ。
     * このタグを持つオブジェクトのみがカスタムパスで描画されます。
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OutlineCapture")
    FName OutlineTargetTag;

    /**
     * @brief Scene Capture Component 2D の出力先となる Render Target。
     * この RT がポストプロセスアウトラインの入力になります。
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OutlineCapture")
    UTextureRenderTarget2D* OutlineRenderTarget;

    /**
     * @brief Scene Capture Component 2D がオブジェクトを描画する際に使用するマテリアル。
     * アウトラインとして表示したいオブジェクトに適用する「白」などの単色マテリアルを設定します。
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OutlineCapture")
    UMaterialInterface* OutlineDrawMaterialOverride;

protected:
    UPROPERTY()
    USceneCaptureComponent2D* SceneCaptureComponent;

    void SetupSceneCapture();
    void UpdateCaptureTargets();
};
