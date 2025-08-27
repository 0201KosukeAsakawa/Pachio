// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ObjectManager.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "FunctionLibrary.h"


void UObjectManager::DuplicateContentsFrom(UObjectManager* Source)
{
    if (!Source)
        return;

    // Engineに頼んで、プロパティ全部コピーしてもらう
    if (GEngine)
    {
        GEngine->CopyPropertiesForUnrelatedObjects(Source, this);
    }
}

void UObjectManager::GenerateObject(FString ActorKey, FString material, FVector location,FVector Scale,FRotator rotation)
{
    // TMap ����w�肳�ꂽ�L�[�Ɋ�Â��ăA�N�^�[�̃N���X��擾
    TSubclassOf<AActor>* ActorClass = FloorActor.Find(ActorKey);

    if (!ActorClass)
        return;
    // �A�N�^�[�̃N���X�����������ꍇ�A�A�N�^�[�𐶐�
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(*ActorClass, location, rotation);
    SpawnedActor->SetActorScale3D(Scale);
    // アクターにアタッチされている全てのコンポーネントを取得
    UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(SpawnedActor, "StaticMesh");
    const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialMap.Find(material);
    if (!MaterialPtr)
        return;
    UMaterialInterface* b = MaterialPtr->LoadSynchronous();

    if (MeshComp && b)
    {
        MeshComp->SetMaterial(0, b);
    }
}