// プロジェクト設定の Description ページに著作権情報を記入

#include "Player/State/PlayerDefaultState.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Player/PlayerCharacter.h"

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

			//マテリアルのサイズを縮小
			StaticMeshComp->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		}
	}

	//コリジョンのサイズ変更
	mOwner->GetCharacterMovement()->Crouch();
	mOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(55.0);

	APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
	if (!aPlayer)
		return false;

	//コリジョン位置調整
	aPlayer->PowerDownCollisionPosition();

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

//ダメージを受けたときの処理
bool UPlayerDefaultState::TakeDamage()
{
	if (!mOwner)
		return false;

	IStateControllable* is = Cast<IStateControllable>(mOwner);
	if (!is)
		return false;

	is->ChangeState("Dead");
	return true;
}
