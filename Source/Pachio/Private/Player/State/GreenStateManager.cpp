#include "Player/State/GreenStateManager.h"
#include "Player/State/PlayerDefaultState.h"
#include "Components/Player/PlayerStateComponent.h"

// コンストラクタ：このコンポーネントが毎フレームTickするように設定
UGreenStateManager::UGreenStateManager()
{
	
}

// ステートマネージャの初期化処理
void UGreenStateManager::Init_Implementation(APawn* Owner, UWorld* World)
{
	UStateManagerBase::Init_Implementation(Owner, World);
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
