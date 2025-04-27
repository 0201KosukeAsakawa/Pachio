// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ObjectManager.h"
#include "Objects/BaseBlock.h"
#include "Engine/Engine.h"


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

void UObjectManager::GenerateObject(FString ActorKey, FVector location, FRotator rotation)
{
    // TMap ����w�肳�ꂽ�L�[�Ɋ�Â��ăA�N�^�[�̃N���X��擾
    TSubclassOf<AActor>* ActorClass = FloorActor.Find(ActorKey);

    if (ActorClass != nullptr)
    {
        // �A�N�^�[�̃N���X�����������ꍇ�A�A�N�^�[�𐶐�
        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(*ActorClass, location, rotation);

        if (SpawnedActor)
        {
            // ���������ꍇ�ɃA�N�^�[�̏������Ȃǂ�s�����Ƃ��ł��܂�
            UE_LOG(LogTemp, Log, TEXT("Actor spawned successfully!"));
        }
        else
        {
            // �������s���̃G���[�n���h�����O
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor!"));
        }
    }
    else
    {
        // �N���X��������Ȃ��ꍇ�̃G���[�n���h�����O
        UE_LOG(LogTemp, Error, TEXT("Actor class not found for key: %s"), *ActorKey);
    }
}

void UObjectManager::GenerateBlock(FString stateID, FString dorpItemID, FString materialID, FVector location, FRotator rotation)
{
    if (BaseBlock == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("BaseBlock class is not set!"));
        return;
    }

    // TSubclassOf<ABaseBlock> ���� UClass* ��擾
    UClass* ActorClass = BaseBlock;

    // �A�N�^�[�̃N���X�����������ꍇ�A�A�N�^�[�𐶐�
    ABaseBlock* SpawnedActor = GetWorld()->SpawnActor<ABaseBlock>(ActorClass, location, rotation);

    if (SpawnedActor)
    {
        // ���������ꍇ�ɃA�N�^�[�̏������Ȃǂ�s�����Ƃ��ł��܂�
        UE_LOG(LogTemp, Log, TEXT("Actor spawned successfully!"));
    }
    else
    {
        // �������s���̃G���[�n���h�����O
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor!"));
    }

    // �A�N�^�[�̏���������
    SpawnedActor->Init(stateID, dorpItemID, materialID);
}
