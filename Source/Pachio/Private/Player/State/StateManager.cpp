#include "Player/State/StateManager.h"
#include "Player/State/PlayerDefaultState.h"
#include "Components/Player/PlayerStateComponent.h"

// コンストラクタ：このコンポーネントが毎フレームTickするように設定
UStateManager::UStateManager()
{
	PrimaryComponentTick.bCanEverTick = true; // Tickを有効にする
}

// ステートマネージャの初期化処理
void UStateManager::Init(APawn* owner, UWorld* world)
{
	// 所有者またはワールドが無効な場合は処理しない
	if (!owner || !world)
		return;

	mOwner = owner;
	pWorld = world;

	// 初期状態を "Default" に設定
	ChangeState(EPlayerStateType::Default);
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

UPlayerStateComponent* UStateManager::ChangeState(EPlayerStateType NextStateTag)
{
	if (StateClassMap.IsEmpty() || !StateClassMap.Contains(NextStateTag) || !mOwner || !pWorld)
		return nullptr;

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
		return nullptr;

	CurrentState->OnEnter(mOwner, pWorld);
	return CurrentState;
}

bool UStateManager::IsStateMatch(EPlayerStateType StateTag)
{
	TSubclassOf<UPlayerStateComponent> StateClass = StateClassMap.FindRef(StateTag);

	if (StateClass == nullptr || CurrentState == nullptr)
	{
		return false;
	}

	return CurrentState->IsA(StateClass);
}
