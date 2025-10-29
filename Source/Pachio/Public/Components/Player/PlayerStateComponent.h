#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "PlayerStateComponent.generated.h"

// 前方宣言（InputActionValue を参照するため）
struct FInputActionValue;
class UPhysicsCalculator;

/**
 * プレイヤーの状態（ステート）を表す基底クラス。
 *
 * 各種派生クラス（移動、ジャンプ、スキルなど）で固有の挙動を実装する。
 * ステート遷移や入力処理を統一的に扱うための抽象コンポーネント。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UPlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** コンストラクタ：デフォルト値を設定 */
	UPlayerStateComponent();

	/**
	 * ステートに入ったときの処理
	 *
	 * @param Owner このステートを持つプレイヤー（Pawn）
	 * @param World 現在のワールド
	 *
	 * @return 成功時 true（ステート遷移を許可）
	 */
	virtual bool OnEnter(APawn* Owner);

	/**
	 * 毎フレームの更新処理（Tick の代わりに呼ばれる）
	 *
	 * @param DeltaTime 前フレームからの経過時間
	 *
	 * @return 継続処理を行う場合 true
	 */
	virtual bool OnUpdate(float DeltaTime);

	/**
	 * ステートから抜けるときの処理
	 *
	 * @param Owner このステートを持つプレイヤー（Pawn）
	 *
	 * @return 成功時 true（正常に終了）
	 */
	virtual bool OnExit(APawn* Owner);

	/**
	 * スキル入力時の処理
	 *
	 * @param Input プレイヤー入力（スキル用アクション値）
	 *
	 * @return 成功時 true（スキル発動）
	 */
	virtual bool OnSkill(const FInputActionValue& Input);

	/**
	 * 移動入力処理
	 *
	 * @param Value プレイヤー入力値（方向や強さ）
	 */
	virtual void Movement(const FInputActionValue& Value);

	/**
	 * ジャンプ処理
	 *
	 * @param jumpForce ジャンプ力
	 *
	 * @return 成功時 true（ジャンプ開始）
	 */
	virtual bool Jump(float jumpForce);

	/**
	 * アニメーション用の速度ベクトルを取得
	 *
	 * @return 現在の移動速度ベクトル
	 */
	FVector GetAnimVelocity() const;

	/**
	 * 現在の移動速度を取得
	 *
	 * @return 移動速度（単位：cm/s）
	 */
	inline virtual float GetMoveSpeed() const { return mMoveSpeed; }

	/**
	 * 現在の移動方向のヨー角を取得
	 *
	 * @return 角度（整数値）
	 */
	int32 GetYaw() const;

protected:
	/** このステートがアタッチされているプレイヤー */
	UPROPERTY()
	APawn* mOwner;

	/** 現在の移動速度（ステートごとに異なる設定が可能） */
	float mMoveSpeed;

	/** 左右の移動方向（1：右 / -1：左） */
	int32 MoveDirection = 1;

	/** 現在の移動変位量（速度×時間） */
	FVector MoveDelta;

	/** ステート時に適用されるマテリアル（ビジュアル演出用） */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterialInterface> NewMaterial;
};
