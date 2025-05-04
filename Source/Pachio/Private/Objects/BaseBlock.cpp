#include "Objects/BaseBlock.h"
#include "Components/BlockState.h"
#include "FunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "DataContainer/BlockDataContainer.h"

// Sets default values
ABaseBlock::ABaseBlock()
{
    // このアクターが毎フレームTickを呼び出すように設定
    // 必要ない場合はオフにしてパフォーマンスを向上させることができる
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseBlock::BeginPlay()
{
    Super::BeginPlay();

    // StateIDとDropItemIDを使って初期化
    Init(StateID, DropItemID);

    // Collisionコンポーネント（BoxComponent）の取得
    Collision = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "Box");

    // Collisionコンポーネントが有効か確認
    if (IsValid(Collision))
    {
        UE_LOG(LogTemp, Warning, TEXT("Collision Component found!"));
        // OnComponentBeginOverlapにコールバックを追加
        Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseBlock::BeginOverlap);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Collision Component not found!"));
    }
}

// 初期化関数
void ABaseBlock::Init(FString stateID, FString dorpItemID, FString materialID)
{
    StateID = stateID;
    DropItemID = dorpItemID;

    // BlockDataContainerがまだない場合、新しく作成
    if (!Container)
    {
        Container = NewObject<UBlockDataContainer>(this, ContainerClass);
    }

    // Containerが有効な場合、現在の状態を設定
    if (Container)
    {
        CurrentState = Container->CreateState(GetWorld(), StateID);
    }

    // 現在の状態が設定されている場合、状態に応じてOnEnter処理を実行
    if (CurrentState)
    {
        if (materialID == "None")
            CurrentState->OnEnter(this, GetWorld(), Container);
        else
            CurrentState->OnEnter(this, GetWorld(), Container, materialID);
    }
}

// Called every frame
void ABaseBlock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 現在は空実装（将来的にブロックの状態更新などを行う場所）
}

// ダメージを受ける処理（現時点ではダメージを受けるロジックはコメントアウト）
bool ABaseBlock::TakeDamage(FAttackData attackData, float damage)
{
    if (attackData.breakLevel == EBreakLevel::Unbreakable)
        return false;

    // 状態が存在する場合、ダメージ処理を行う（現在はコメントアウト）
    if (CurrentState)
    {
        CurrentState->OnHit(attackData,FVector(0, 0, 0));
    }
    return true;
}

// 状態を変更する処理
void ABaseBlock::ChangeState(UBlockState* nextState)
{
    // 現在の状態が存在する場合、終了処理を行う
    if (CurrentState)
        CurrentState->OnExit(this);

    // 次の状態が指定されている場合、状態を変更
    if (nextState)
        CurrentState = nextState;

    // Containerが有効な場合、次の状態に対してOnEnterを呼び出す
    if (Container)
        CurrentState->OnEnter(this, GetWorld(), Container);
}

// 衝突時に呼ばれる処理
void ABaseBlock::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 状態が存在する場合、OnHitを呼び出す
    //if (CurrentState)
    //{
    //    CurrentState->OnHit(OtherActor, FVector(0, 0, 0));
    //}
}
