// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OutlineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibrary.h"
UOutlineComponent::UOutlineComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // Tickを有効化
}

void UOutlineComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UStaticMeshComponent* OriginalMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(Owner,"StaticMesh");
    if (!OriginalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("OriginalMesh not found"));
        return;
    }

    if (!OutlineMesh)
    {
        OutlineMesh = NewObject<UStaticMeshComponent>(Owner, TEXT("OutlineMesh"));
        if (!OutlineMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create OutlineMesh"));
            return;
        }
    }

    OutlineMesh->SetStaticMesh(OriginalMesh->GetStaticMesh());
    OutlineMesh->AttachToComponent(OriginalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
    OutlineMesh->RegisterComponent();

    // スケールアップ（アウトラインの太さ調整）
    OutlineMesh->SetRelativeScale3D(FVector(1.05f));
    OutlineMesh->SetRelativeLocation(FVector::ZeroVector);
    OutlineMesh->SetRelativeRotation(FRotator::ZeroRotator);

    if (OutlineMaterial)
    {
        OutlineMesh->SetMaterial(0, OutlineMaterial);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OutlineMaterial is not set"));
    }

    OutlineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    OutlineMesh->SetVisibility(true);

    // 手前に描画したいので優先度を大きくする
    OutlineMesh->TranslucencySortPriority = 1000;

    // マテリアル側でDisable Depth TestをONにしておくこと（重要）

    OutlineMesh->SetRenderCustomDepth(false);

    OutlineMesh->MarkRenderStateDirty();
    OutlineMesh->MarkRenderDynamicDataDirty();

    UE_LOG(LogTemp, Warning, TEXT("OutlineMesh Visible: %s, Registered: %s"),
        OutlineMesh->IsVisible() ? TEXT("true") : TEXT("false"),
        OutlineMesh->IsRegistered() ? TEXT("true") : TEXT("false"));
}

void UOutlineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // カメラ位置を取得（プレイヤーのビューアクター）
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // オブジェクトの位置（判定用）
    FVector ObjectLocation = Owner->GetActorLocation();

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner); // 自分自身は無視（必要に応じて外す）

    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        CameraLocation,
        ObjectLocation,
        ECC_Visibility,
        Params);

    bool bIsBehind = false;

    if (bHit)
    {
        // ヒットしたのが自分じゃないなら裏に隠れている
        if (HitResult.GetActor() != Owner)
        {
            bIsBehind = true;
        }
    }
    else
    {
        // ヒットしなかったら見えている
        bIsBehind = false;
    }

    if (OutlineMesh)
    {
        OutlineMesh->SetVisibility(bIsBehind);
    }
}