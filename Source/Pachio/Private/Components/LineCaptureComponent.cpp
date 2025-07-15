// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LineCaptureComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h" 
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h" // UMaterialInstanceDynamic を使うなら必要

ULineCaptureComponent::ULineCaptureComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    OutlineTargetTag = FName("OutlineTarget"); // 例: "OutlineTarget"タグを持つアクターを対象
    OutlineRenderTarget = nullptr; // エディタでアセットを割り当てることを想定
    OutlineDrawMaterialOverride = nullptr; // Hogeマテリアルなど。USceneCaptureComponent2Dでは直接使用しない
}

void ULineCaptureComponent::BeginPlay()
{
    Super::BeginPlay();
    SetupSceneCapture();
}

void ULineCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateCaptureTargets();
}

void ULineCaptureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SceneCaptureComponent)
    {
        SceneCaptureComponent->DestroyComponent();
        SceneCaptureComponent = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}

void ULineCaptureComponent::SetupSceneCapture()
{
    if (!SceneCaptureComponent)
    {
        AActor* OwnerActor = GetOwner();
        if (OwnerActor)
        {
            SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(OwnerActor, TEXT("OutlineSceneCapture"));
            if (SceneCaptureComponent)
            {
                SceneCaptureComponent->SetupAttachment(OwnerActor->GetRootComponent());
                SceneCaptureComponent->RegisterComponent();

                // ShowOnlyList を使用して、指定されたアクターのみをレンダリングする
                SceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

                // ★最重要: SCS_CustomDepth をキャプチャソースとして設定する★
                // これにより、カスタムデプスパスの情報（深度とステンシル）がキャプチャされる
                //SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_CustomDepth;

                SceneCaptureComponent->bCaptureEveryFrame = false; // TickComponentで手動でキャプチャ
                SceneCaptureComponent->bAlwaysPersistRenderingState = true;
                SceneCaptureComponent->bUseCustomProjectionMatrix = false;

                // Render Target の設定。エディタで割り当てがない場合は動的に作成
                if (OutlineRenderTarget)
                {
                    SceneCaptureComponent->TextureTarget = OutlineRenderTarget;
                }
                else
                {
                    OutlineRenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("OutlineRenderTarget_Dynamic"));
                    // ★Render Target Format を PF_R32_FLOAT に設定する★
                    // SCS_CustomDepth でキャプチャされるデータ（深度とステンシル）を効率的に格納するため。
                    // 深度はRチャンネルに、ステンシルはGチャンネルに格納されることが多いが、
                    // PF_R32_FLOAT は単一チャンネルなので、後でマテリアルでGチャンネルを読むことでステンシルにアクセスできるか確認する。
                    // もし問題があれば PF_B8G8R8A8 に変更も検討する。
                    OutlineRenderTarget->InitCustomFormat(1024, 768, PF_R32_FLOAT, false);
                    OutlineRenderTarget->ClearColor = FLinearColor::Black; // クリアカラーは黒（デフォルト）
                    OutlineRenderTarget->UpdateResourceImmediate(true);
                    SceneCaptureComponent->TextureTarget = OutlineRenderTarget;
                }

                // ★重要: USceneCaptureComponent2D の PostProcessSettings を完全に空にする★
                // Hogeマテリアルはここには適用せず、メインビューのPost Process Volume で使用します。
                SceneCaptureComponent->PostProcessSettings.WeightedBlendables.Array.Empty();
            }
        }
    }
}

void ULineCaptureComponent::UpdateCaptureTargets()
{
    if (!SceneCaptureComponent || !GetWorld())
    {
        return;
    }

    // CaptureScene() の前に ShowOnlyActors と HiddenActors をクリア
    SceneCaptureComponent->ShowOnlyActors.Empty();
    SceneCaptureComponent->HiddenActors.Empty();

    // OutlineTargetTag を持つアクターを検索
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), OutlineTargetTag, FoundActors);

    // 見つかったアクターのプリミティブコンポーネントを処理
    for (AActor* Actor : FoundActors)
    {
        TArray<UPrimitiveComponent*> PrimitiveComponents;
        Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
        for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
        {
            if (PrimComp && PrimComp->IsVisible())
            {
                // ★重要: CustomDepth を有効にし、Stencil Value を「5」に設定する★
                PrimComp->SetRenderCustomDepth(true);
                PrimComp->SetCustomDepthStencilValue(5); // ここを「5」に設定

                // このアクターを ShowOnlyList に追加
                // SCS_CustomDepth の場合でも ShowOnlyList は適用され、指定されたアクターのCustomDepthのみをキャプチャする
                SceneCaptureComponent->ShowOnlyActors.Add(Actor);
            }
        }
    }

    // シーンキャプチャコンポーネントの位置と回転をプレイヤーカメラに合わせる
    APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    if (PCM)
    {
        SceneCaptureComponent->SetWorldLocationAndRotation(PCM->GetCameraLocation(), PCM->GetCameraRotation());
    }

    // シーンをキャプチャしてRender Targetに書き込む
    SceneCaptureComponent->CaptureScene();

    // ★重要: CaptureScene() の後で CustomDepth を無効に戻す★
    // これをしないと、メインビューのレンダリングにも CustomDepth が影響を与えてしまう（例: オブジェクトが透けて見えるなど）
    for (AActor* Actor : FoundActors)
    {
        TArray<UPrimitiveComponent*> PrimitiveComponents;
        Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
        for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
        {
            if (PrimComp && PrimComp->IsVisible())
            {
                PrimComp->SetRenderCustomDepth(false); // CustomDepthを無効に戻す
                PrimComp->SetCustomDepthStencilValue(0); // Stencil Valueもリセット
            }
        }
    }
}