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
    OutlineMesh->SetRelativeScale3D(FVector(10.05f));
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
