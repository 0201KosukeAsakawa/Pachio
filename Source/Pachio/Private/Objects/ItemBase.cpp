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
    // USphereComponentはコリジョン検出を行うコンポーネント
    USphereComponent* CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;  // RootComponentとして設定
    CollisionComponent->InitSphereRadius(50.0f);  // 半径50で初期化
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 動的オーバーラップ用のプロファイルを設定

    // OnCollected イベントにバインド
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnCollected);  // 重なった際にOnCollected関数を呼び出す
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
    Super::BeginPlay();

    // ItemLogicをSuperMushroomComponentで初期化
    ItemLogic = NewObject<UItemEffectSourceComponent>(this);

    // PhysicsCalculatorコンポーネントを探して初期化
    physics = UFunctionLibrary::FindComponentByName<UPhysicsCalculator>(this, "BP_Physics");

    // ItemLogicが有効であれば初期化
    if (ItemLogic)
    {
        ItemLogic->Init(this);
        ItemLogic->SetDirection(direction);  // directionで向きを設定
    }
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ItemLogicが存在すればアップデート
    if (ItemLogic)
        ItemLogic->Update(DeltaTime);
}

// アイテムの初期化を行う関数
void AItemBase::Init(FString objectID, const FString meshID, const FString materialID, const FVector direc)
{
    ObjectID = objectID;

    // ObjectIDが"None"ならば何もしない
    if (ObjectID == "None")
        return;

    // LevelManagerが存在すればItemContainerからアイテムの状態を作成
    if (ALevelManager::GetInstance(GetWorld()))
    {
        ItemLogic = ALevelManager::GetInstance(GetWorld())->GetItemContainer()->CreateState(GetWorld(), ObjectID);
    }

    // アイテムのロジックが存在すれば初期化
    if (ItemLogic)
    {
        ItemLogic->Init(this);
        ItemLogic->SetDirection(direc);  // アイテムの向きを設定
    }
}

// 力を加える関数
void AItemBase::AddForce(float force, FVector addDirection)
{
    // PhysicsCalculatorコンポーネントを使用して力を加える
    GetPhysics()->AddForce(addDirection, force, true);
}

// アイテムが重なったときに呼ばれる関数
void AItemBase::OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // ItemLogicが存在すればOnCollectedイベントを処理
    if (ItemLogic)
    {
        ItemLogic->OnCollected(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    }
}