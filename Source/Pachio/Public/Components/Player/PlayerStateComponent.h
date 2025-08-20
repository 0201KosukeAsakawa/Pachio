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
 * 派生クラスで各種挙動（移動、スキル、ジャンプなど）をオーバーライド可能。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UPlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UPlayerStateComponent();

	// ステートに入ったときの処理
	virtual bool OnEnter(ACharacter* Owner, UWorld* World);

	// 毎フレームの更新処理（Tick の代わりに呼ばれる）
	virtual bool OnUpdate(float DeltaTime);

	// ステートから出るときの処理
	virtual bool OnExit(ACharacter* Owner);

	// スキル入力時の処理
	virtual bool OnSkill(const FInputActionValue& Input);

	virtual void Movement(const FInputActionValue& Value);

	virtual void Jump(UPhysicsCalculator* physics, float jumpForce);
	
	//移動速度
	inline virtual float GetMoveSpeed()const { return mMoveSpeed; }

protected:
	/** このステートがアタッチされているキャラクター */
	UPROPERTY()
	ACharacter* mOwner;

	/** アクセス可能なワールドオブジェクト */
	UPROPERTY()
	UWorld* pWorld;

	/** 現在の移動速度（ステートごとに設定可能） */
	float mMoveSpeed;

	/** ステートごとに適用されるマテリアル（ビジュアル変更用） */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterialInterface> NewMaterial;
};