// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockonWidget.generated.h"

class UImage;
/**
 * @brief ロックオン対象を表示するUIウィジェット
 * AActor をターゲットとして画面上にマーカー表示を行う
 */
UCLASS()
class PACHIO_API ULockonWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief ロックオン対象のアクターを設定
     * @param InActor ターゲットにするアクター
     */
    void SetTargetActor(AActor* InActor) { TargetActor = InActor; }

    /**
     * @brief 現在のターゲットアクターを取得
     * @return AActor* ターゲットアクターのポインタ
     */
    AActor* GetTargetActor() const { return TargetActor; }

    /**
     * @brief マーカーの画面上の位置を更新
     * @param ScreenPosition 画面座標上の位置（ピクセル単位）
     */
    void UpdateScreenPosition(const FVector2D& ScreenPosition);

    /**
     * @brief マーカーの表示/非表示を設定
     * @param bVisible trueで表示、falseで非表示
     */
    void SetMarkerVisible(bool bVisible);

protected:
    /** @brief UMGでバインドされたロックオン画像 */
    UPROPERTY(meta = (BindWidget))
    UImage* LockonImage;

    /** @brief マーカーの画面位置補正用ベクトルスケール */
    UPROPERTY(EditAnywhere)
    FVector2D AddVectorScale = FVector2D(0, 0);

private:
    /** @brief ロックオン対象のアクター */
    UPROPERTY()
    AActor* TargetActor;
};
