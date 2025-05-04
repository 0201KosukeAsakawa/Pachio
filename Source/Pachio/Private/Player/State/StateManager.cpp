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

bool UStateManager::ChangeState(FString NextStateTag)
{
	if (StateClassMap.IsEmpty() || !StateClassMap.Contains(NextStateTag) || !mOwner || !pWorld)
		return false;

	TSubclassOf<UPlayerStateComponent> StateClass = StateClassMap[NextStateTag];

	// 既存ステートを終了
	if (CurrentState)
	{
		CurrentState->OnExit(mOwner);
		CurrentState->ConditionalBeginDestroy(); // メモリ解放（必要に応じて）
		CurrentState = nullptr;
	}

	// 新しいステートを生成
	CurrentState = NewObject<UPlayerStateComponent>(mOwner, StateClass);
	if (!CurrentState)
		return false;

	CurrentState->OnEnter(mOwner, pWorld);
	return true;
}