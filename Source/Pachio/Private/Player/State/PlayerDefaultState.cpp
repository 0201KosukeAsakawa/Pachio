// プロジェクト設定の Description ページに著作権情報を記入

#include "Player/State/PlayerDefaultState.h"
#include "Player/PlayerCharacter.h"
#include "Player/InGameController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MoveComponent.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Logic/Movement/PlayerMoveLogic.h"

// ステートに入る際に実行される処理
bool UPlayerDefaultState::OnEnter(ACharacter* owner, UWorld* world)
{
	// 所有キャラクターまたはワールドが無効な場合は失敗
	if (owner == nullptr || world == nullptr)
	{
		return false;
	}

	// 内部に所有者とワールドを保存
	if(!mOwner)
	mOwner = owner;
	if(!pWorld)
	pWorld = world;
	if(!MoveComp)
	{
		MoveComp = NewObject<UMoveComponent>(mOwner);
		UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
		MoveComp->Init(PlayerLogic);
	}
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

	//コリジョンのサイズ変更
	mOwner->GetCharacterMovement()->Crouch();
	mOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(55.0);

	APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
	if (!aPlayer)
		return false;


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
bool UPlayerDefaultState::OnSkill(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) // 入力が有効な場合（ボタンが押された場合など）
	{
		// このPawnを操作しているコントローラーを取得
		AController* OwningController = mOwner->GetController();
		if (OwningController)
		{
			// AInGameController にキャスト（もし AInGameController がこのPlayerCharacterをPossessしている場合）
			AInGameController* InGameController = Cast<AInGameController>(OwningController);
			if (InGameController)
			{
				// コントローラーのTogglePossession関数を呼び出す
				InGameController->TogglePossession(mOwner);
			}
		}
	}

	return true;
}

void UPlayerDefaultState::Movement(const FInputActionValue& Value)
{
	// 移動方向をMoveCompのロジックから取得
	FVector direction = MoveComp->Movement(0, mOwner, Value);
	// 速度は現在のステートが持つ移動速度を使用
	mOwner->AddMovementInput(direction, MoveSpeed);
}