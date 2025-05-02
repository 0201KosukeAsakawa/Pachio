// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ItemBase.h"
#include "Objects/SuperMushroom.h"
#include "DataContainer/ItemDataContainer.h"
#include "Components/SphereComponent.h"
#include "Interface/ItemEffectSource.h"
#include "Components/PhysicsCalculator.h"
#include "Manager/LevelManager.h"
#include "FunctionLibrary.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // スフィアコリジョンで重なった際にイベントをバインド
    USphereComponent* CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // OnCollected にバインド
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnCollected);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
    Super::BeginPlay();
    ItemLogic = NewObject<USuperMushroomComponent>(this);
    physics = UFunctionLibrary::FindComponentByName<UPhysicsCalculator>(this, "BP_Physics");
    if (ItemLogic)
    {
        ItemLogic->Init(this);
        ItemLogic->SetDirection(direction);
    }
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ItemLogic)
        ItemLogic->Update(DeltaTime);
}

void AItemBase::Init(FString objectID, const FString meshID, const FString materialID, const FVector direc)
{
    ObjectID = objectID;

    if (ObjectID == "None")
        return;

    if (ALevelManager::GetComponent(GetWorld()))
    {
        ItemLogic = ALevelManager::GetComponent(GetWorld())->GetItemContainer()->CreateState(GetWorld(), ObjectID);
    }
    // �X�e�[�g�̏�����
    if (ItemLogic)
    {
        ItemLogic->Init(this);

        ItemLogic->SetDirection(direc);
    }
}

void AItemBase::AddForce(float force , FVector addDirection)
{
    GetPhysics()->AddForce(addDirection, force , true);
}

void AItemBase::OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ItemLogic)
    {
        ItemLogic->OnCollected(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    }
}

