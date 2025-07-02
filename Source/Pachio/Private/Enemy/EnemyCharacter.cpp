#include "Enemy/EnemyCharacter.h"
#include "DataContainer/EnemyDataContainer.h"
#include "Manager/LevelManager.h"
#include "Manager/ScoreManager.h"
#include "Components/BoxComponent.h"
#include "FunctionLibrary.h"

// コンストラクタ：コンポーネントの初期化
// コンストラクタ: 初期設定を行う
AEnemyCharacter::AEnemyCharacter()
{
    // 毎フレーム Tick を呼び出すように設定
    PrimaryActorTick.bCanEverTick = true;
}

// ゲーム開始時に呼び出される初期化処理
void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // メッシュコンポーネントを探し、meshComponentに設定
    meshComponent = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("MeshComp"));

    // メッシュコンポーネントをルートコンポーネントとして設定
    RootComponent = meshComponent;
}

// 初期化処理
void AEnemyCharacter::Init(const EEnemyCategory logicID,const EEnemyCategory materialID)
{

}

// 毎フレーム実行される更新処理
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

  
}

// 当たり判定開始時の処理（他のアクターとの衝突時に呼び出される）
void AEnemyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 自分自身または無効な相手を無視
    if (OtherActor == this || !OtherActor)
        return;

    // プレイヤーの攻撃判定と衝突した場合は無視
    if (OtherComp->ComponentHasTag("Attack"))
        return;
}
