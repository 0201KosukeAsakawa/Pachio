// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/BlockDepletedState.h"
#include "Attack/AttackStrategy.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "FunctionLibrary.h"

bool UBlockDepletedState::OnEnter(AActor* owner, UWorld*)
{
    if (!owner)
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
    return true;
}
bool UBlockDepletedState::OnUpdate(AActor*)
{
    return true;
}
bool UBlockDepletedState::OnExit(AActor*)
{
    return true;
}
bool UBlockDepletedState::OnHit(FVector, FAttackData)
{
    return true;
}