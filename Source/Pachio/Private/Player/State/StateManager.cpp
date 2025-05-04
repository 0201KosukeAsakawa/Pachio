#include "Player/State/StateManager.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/State/PlayerSuperState.h"
#include "Components/PlayerStateComponent.h"

// コンストラクタ：このコンポーネントが毎フレームTickするように設定
UStateManager::UStateManager()
{
	PrimaryComponentTick.bCanEverTick = true; // Tickを有効にする
}

// ステートマネージャの初期化処理
void UStateManager::Init(ACharacter* owner, UWorld* world)
{
	// 所有者またはワールドが無効な場合は処理しない
	if (!owner || !world)
		return;

	mOwner = owner;
	pWorld = world;

	// 各ステートのインスタンスを生成
	UPlayerDefaultState* Default = NewObject<UPlayerDefaultState>(mOwner); // 通常状態
	UPlayerSuperState* Super = NewObject<UPlayerSuperState>(mOwner);       // スーパー状態（強化状態）

	// ステートマップに登録（文字列タグをキーに）
	StateMap.Add("Default", Default);
	StateMap.Add("Super", Super);
	// 今後 "Fire" や "Jumping" などを追加していくことも可能

	// 初期状態を "Default" に設定
	ChangeState("Default");
}

// 毎フレームの更新処理（Tickなどから呼び出される想定）
void UStateManager::Update(float deltaTime)
{
	// 現在のステートが存在する場合、ステートのOnUpdateを呼び出す
	if (CurrentState != nullptr)
	{
		CurrentState->OnUpdate(deltaTime);
	}
}

// ステートを指定された名前（タグ）に切り替える
bool UStateManager::ChangeState(FString nextState)
{
	// 次のステートをマップから取得
	UPlayerStateComponent* next = StateMap[nextState];
	if (!next || !mOwner || !pWorld)
		return false;

	// 現在のステートがある場合はOnExitでクリーンアップ
	if (CurrentState)
	{
		CurrentState->OnExit(mOwner);
	}

	// 新しいステートに切り替え
	CurrentState = next;
	CurrentState->OnEnter(mOwner, pWorld); // 新ステートの初期化処理

	return true;
}