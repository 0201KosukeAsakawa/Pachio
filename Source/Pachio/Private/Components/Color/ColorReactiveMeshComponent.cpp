// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorReactiveMeshComponent.h"
#include "ColorUtilityLibrary.h"


UColorReactiveMeshComponent::UColorReactiveMeshComponent()
{
}

void UColorReactiveMeshComponent::ApplyColorWithMatching(const FLinearColor& NewColor)
{
    // 1️ 色から最も近い色の種類を取得
    EColorCategory Category = UColorUtilityLibrary::GetNearestPrimaryColor(NewColor);

    // 2️ 対応するメッシュが存在するか確認
    if (!MeshData.Contains(Category))
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyColorWithMatching: No mesh found for color category %d"), (int32)Category);
        return;
    }

    USkeletalMesh* ColorMesh = MeshData[Category].Mesh;
    if (!ColorMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyColorWithMatching: Mesh for category %d is null"), (int32)Category);
        return;
    }

    // 3️ オーナー取得
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyColorWithMatching: Owner is null"));
        return;
    }

    // 4️ スケルタルメッシュコンポーネント取得
    USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyColorWithMatching: Owner has no SkeletalMeshComponent"));
        return;
    }

    // 5️ メッシュとアニメBPを適用
    MeshComp->SetSkeletalMesh(ColorMesh);

    if (MeshData[Category].AnimBP)
    {
        MeshComp->SetAnimInstanceClass(MeshData[Category].AnimBP);
    }

    UE_LOG(LogTemp, Log, TEXT("ApplyColorWithMatching: Applied mesh for category %d"), (int32)Category);
}

