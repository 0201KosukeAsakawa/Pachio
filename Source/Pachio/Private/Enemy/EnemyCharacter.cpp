#include "Enemy/EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Enemy/State/GoombaStateComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ScoreManager.h"

// コンストラクタ：コンポーネントの初期化
AEnemyCharacter::AEnemyCharacter()
{
	// 毎フレーム Tick を呼び出すように設定
	PrimaryActorTick.bCanEverTick = true;

	// 当たり判定用のボックスコンポーネントを作成し、ルートに設定
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComponent"));
	RootComponent = BoxComponent;

	// 見た目のメッシュコンポーネントを作成し、当たり判定にアタッチ
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

// ゲーム開始時に呼び出される初期化処理
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 敵の状態を管理するロジックコンポーネントを生成
	Logic = NewObject<UGoombaStateComponent>(this);

	// 正常に生成できていれば、初期化を実行
	if (Logic)
	{
		Logic->OnEnter(this);
	}
}

// 毎フレーム実行される処理
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ロジックが有効なら更新処理を実行
	if (Logic)
	{
		Logic->OnUpdate(DeltaTime);
	}
}

// ダメージを受けたときの処理（IDamageable インターフェイスの実装）
bool AEnemyCharacter::TakeDamage(FAttackData Data, float damage)
{
	HP -= damage;

	ALevelManager::GetInstance(GetWorld())->GetScoreManager()->AddScore(Score);

	return true;
}

// 生存状態かどうかの判定（IDamageable インターフェイスの実装）
bool AEnemyCharacter::IsDead() const
{
	// HP が 0 以下なら死亡扱い
	return HP <= 0;
}
