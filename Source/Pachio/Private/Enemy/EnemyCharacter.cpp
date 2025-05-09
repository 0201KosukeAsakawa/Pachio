#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Enemy/State/GoombaStateComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ScoreManager.h"
#include "Components/BoxComponent.h"
#include "FunctionLibrary.h"

// コンストラクタ：コンポーネントの初期化
AEnemyCharacter::AEnemyCharacter()
{
	// 毎フレーム Tick を呼び出すように設定
	PrimaryActorTick.bCanEverTick = true;
}

// ゲーム開始時に呼び出される初期化処理
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 当たり判定用のボックスコンポーネントを作成し、ルートに設定
	AttackCollision = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "C_Attack");

	// 敵の状態を管理するロジックコンポーネントを生成
	Logic = NewObject<UGoombaStateComponent>(this);

	// 正常に生成できていれば、初期化を実行
	if (!Logic || !AttackCollision)
		return;

	Logic->OnEnter(this, GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("AttackCollision is valid"));
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapBegin);

}

// 毎フレーム実行される処理
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ロジックが有効なら更新処理を実行
	if (!Logic)
		return;

	Logic->OnUpdate(DeltaTime);

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

void AEnemyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 自分自身 or 無効な相手を無視
	if (OtherActor == this || !OtherActor)
		return;

	// プレイヤーの攻撃判定と衝突した場合は無視
	if (OtherComp->ComponentHasTag("PlayerAttack"))
		return;

	if (Logic)
		Logic->OnOverlap(OtherActor);
}
