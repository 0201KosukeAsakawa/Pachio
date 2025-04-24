// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/EmptyState.h"
#include "Components/StaticMeshComponent.h"
#include "FunctionLibrary.h"

bool UEmptyState::OnEnter(ABaseBlock* owner, UWorld*, UBlockDataContainer*c)
{
    if (!owner || !c)
        return false;
    mOwner = owner;

    // アクターにアタッチされている全てのコンポーネントを取得
    UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");
    if (!NewMaterial)
    {
        // 新しいマテリアルを取得（例えば、ゲームのアセットから）
        NewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/NewMaterial.NewMaterial'"));
    }

    if (NewMaterial && MeshComp)
    {
        // マテリアルを変更
        MeshComp->SetMaterial(0, NewMaterial); // 0 はマテリアルのインデックス
    }
    Container = c;
    return true;
}
