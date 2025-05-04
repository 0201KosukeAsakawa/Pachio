// プロジェクト設定の Description ページに著作権情報を記入

#include "Player/State/PlayerDefaultState.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"

// ステートに入る際に実行される処理
bool UPlayerDefaultState::OnEnter(ACharacter* owner, UWorld* world)
{
	// 所有キャラクターまたはワールドが無効な場合は失敗
	if (owner == nullptr || world == nullptr)
	{
		return false;
	}

	// 内部に所有者とワールドを保存
	mOwner = owner;
	pWorld = world;

	// マテリアルの設定（デフォルトステート用）
	//if (NewMaterial)
	{
		// キャラクターが持つ StaticMeshComponent を取得
		UStaticMeshComponent* StaticMeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(owner, "StaticMesh");
		UMaterialInterface* N = NewMaterial.LoadSynchronous(); // 非同期ロードに対応
		if (N != nullptr && StaticMeshComp)
		{
			StaticMeshComp->SetMaterial(0, N); // マテリアルをスロット0に適用
		}
	}

	// 移動速度の初期値設定（ステート内で使用）
	mMoveSpeed = 100.0f;

	return true; // ステートの切り替え成功
}

// ステートの毎フレーム更新処理（現時点では何もしない）
bool UPlayerDefaultState::OnUpdate(float)
{
	return true;
}

// ステートを離脱するときの処理（現時点では何もしない）
bool UPlayerDefaultState::OnExit(ACharacter*)
{
	return true;
}

// スキルボタン入力時の処理（現時点では何もしない）
bool UPlayerDefaultState::OnSkill(const FInputActionValue&)
{
	return true;
}
