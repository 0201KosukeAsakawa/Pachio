// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameHUD.h"
#include "UI/UIManager.h"
#include "UI/LockonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void AInGameHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // UIManager が存在しなければ処理しない
    if (!UIManager)
        return;

    // UIManager が管理している全てのロックオンマーカーを取得
    const TMap<FName, ULockonWidget*>& Markers = UIManager->GetAllMarkers();

    // この HUD を所有している PlayerController を取得
    APlayerController* PlayerController = GetOwningPlayerController();
    if (!PlayerController)
        return;

    // 現在のウィジェットスケール（DPIスケーリング対応）
    float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

    // 登録されているマーカーを順番に処理
    for (const auto& Pair : Markers)
    {
        ULockonWidget* MarkerWidget = Pair.Value;

        // マーカーが無効または表示されていない場合はスキップ
        if (!MarkerWidget || !MarkerWidget->IsInViewport())
            continue;

        // マーカーが追従しているターゲットアクターを取得
        AActor* Target = MarkerWidget->GetTargetActor();
        if (!Target)
            continue;

        FVector2D ScreenPos;

        // ターゲットのワールド座標をスクリーン座標に変換
        if (PlayerController->ProjectWorldLocationToScreen(Target->GetActorLocation(), ScreenPos))
        {
            // DPIスケーリングを補正
            ScreenPos /= Scale;

            // マーカーの UI 表示位置を更新
            MarkerWidget->UpdateScreenPosition(ScreenPos);
        }
    }
}
