#include "Player/State/GreenStateManager.h"
#include "Player/State/PlayerDefaultState.h"
#include "Components/Player/PlayerStateComponent.h"

// コンストラクタ：このコンポーネントが毎フレームTickするように設定
UGreenStateManager::UGreenStateManager()
{
	
}

// ステートマネージャの初期化処理
void UGreenStateManager::Init_Implementation(APawn* owner, UWorld* world)
{
	// 所有者またはワールドが無効な場合は処理しない
	if (!owner || !world)
		return;
	mOwner = owner;
	// 初期状態を "Default" に設定
	Execute_ChangeState(this,EPlayerStateType::Default);
}

// 毎フレームの更新処理（Tickなどから呼び出される想定）
void UGreenStateManager::Update_Implementation(float deltaTime)
{
	// 現在のステートが存在する場合、ステートのOnUpdateを呼び出す
	if (CurrentState != nullptr)
	{
		CurrentState->OnUpdate(deltaTime);
	}
}

UPlayerStateComponent* UGreenStateManager::ChangeState_Implementation(EPlayerStateType NextStateTag)
{
	if (StateClassMap.IsEmpty() || !StateClassMap.Contains(NextStateTag)|| !mOwner)
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

	CurrentState->OnEnter(mOwner, GetWorld());
	return CurrentState;
}

bool UGreenStateManager::IsStateMatch_Implementation(EPlayerStateType StateTag)
{
	TSubclassOf<UPlayerStateComponent> StateClass = StateClassMap.FindRef(StateTag);

	if (StateClass == nullptr || CurrentState == nullptr)
	{
		return false;
	}

	return CurrentState->IsA(StateClass);
}
