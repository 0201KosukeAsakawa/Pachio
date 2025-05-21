#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Enemy/State/GoombaStateComponent.h"
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

    // 当たり判定用のボックスコンポーネントを探し、AttackCollisionに設定
    AttackCollision = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "C_Attack");

    // メッシュコンポーネントを探し、meshComponentに設定
    meshComponent = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("MeshComp"));

    // メッシュコンポーネントをルートコンポーネントとして設定
    RootComponent = meshComponent;

    // ロジックIDとマテリアルIDを使って初期化
    Init(LogicID, MaterialID);

    // ロジックまたは当たり判定が無効なら、処理を中断
    if (!Logic || !AttackCollision)
        return;

    // AttackCollisionが正常に設定されていれば、OnOverlapBeginイベントをバインド
    UE_LOG(LogTemp, Warning, TEXT("AttackCollision is valid"));
    AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapBegin);
}

// 初期化処理
void AEnemyCharacter::Init(FString logicID, const FString materialID)
{
    // LogicIDを設定
    LogicID = logicID;

    // EnemyContainerからLogic状態を作成して設定
    Logic = ALevelManager::GetInstance(GetWorld())->GetEnemyContainer()->CreateState(GetWorld(), LogicID);

    // Logicが無効なら、処理を中断
    if (!Logic)
        return;

    // マテリアルIDが "None" でない場合、ロジックにマテリアルIDを渡してOnEnter処理を実行
    if (materialID == "None")
        Logic->OnEnter(this, GetWorld());
    else
        Logic->OnEnter(this, GetWorld(), materialID);
}

// 毎フレーム実行される更新処理
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Logicが無効なら更新処理を実行しない
    if (!Logic)
        return;

    // LogicのOnUpdateメソッドを呼び出して、毎フレームの更新を実行
    Logic->OnUpdate(DeltaTime);
}

// ダメージを受けたときの処理（IDamageable インターフェイスの実装）
bool AEnemyCharacter::TakeDamage(FAttackData Data, float damage , const AActor*)
{
    // HPからダメージを減算
    HP -= damage;

    // スコアを加算
    ALevelManager::GetInstance(GetWorld())->GetScoreManager()->AddScore(Score);

    // ダメージ処理が成功したことを返す
    return true;
}

// 生存状態かどうかの判定（IDamageable インターフェイスの実装）
bool AEnemyCharacter::IsDead() const
{
    // HPが0以下なら死亡扱い
    return HP <= 0;
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

    // ロジックが有効であれば、そのロジックのOnOverlap処理を呼び出す
    if (Logic)
        Logic->OnOverlap(OtherActor);
}
