// プレイヤーキャラクターの基本クラス
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/IDamageable.h"
#include "Interface/StateControllable.h"
#include "PlayerCharacter.generated.h"

// ===========================
// 前方宣言
// ===========================
class IStateBase;
class IMoveLogic;
class UPlayerDefaultState;
class UInputMappingContext;
class UInputAction;
class UStateManager;
class UAttackComponent;
class UAttackController;
class USpringArmComponent;
class UCameraComponent;
class UColorControllerComponent;
class UBoxComponent;
class UCameraHandlerComponent;
class UInvincibilityComponent;

class UPhysicsCalculator;
class UMoveComponent;

class UPlayerInputComponent;


struct FInputActionValue;
/**
 * APlayerCharacter
 * プレイヤーキャラクターの基底クラス。
 * 入力処理、ステート遷移、カメラ制御、攻撃衝突判定などの主要機能を実装。
 */
UCLASS()
class PACHIO_API APlayerCharacter : public ACharacter, public IStateControllable, public IDamageable
{
	GENERATED_BODY()

public:
	// デフォルトコンストラクタ
	APlayerCharacter();

protected:
	// ゲーム開始時に一度だけ呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム更新
	virtual void Tick(float DeltaTime) override;

	// プレイヤー入力のバインディング処理
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPlayerStateComponent* GetPlayerState() const override;

public:
	// ==== 入力アクション ====
	// 移動入力
	void Movement(const FInputActionValue& Value);

	// ジャンプ開始・終了
	void Jump(const FInputActionValue& Value);
	void JumpStop(const FInputActionValue& Value);

	// ダッシュ（特殊アクション）開始・終了
	void Action(const FInputActionValue& Value);
	void StopAction();

	void DecreaseColor();
	void IncreaseColor();

	void ShiftArrayRightColorMode();
	void ShiftArrayLeftColorMode();
private:
	// ==== 攻撃コリジョン ====

	// ステートの変更（タグ指定）
	bool ChangeState(FString Tag)override;
	void ApplyEffectFromColor(const FLinearColor& Color);
	bool TakeDamage(FAttackData Data, const float damage = 0, const AActor* = nullptr)override;

private:
	// ==== 状態・戦闘 ====
	UPROPERTY()
	UCameraHandlerComponent* CameraComponent;

	UPROPERTY()
	UInvincibilityComponent* InvincibilityComponent;

	//ステート管理のクラス
	UPROPERTY(EditAnywhere, Category = "State")
	TSubclassOf<UStateManager> StateManagerClass;

	// 状態管理コンポーネント
	UPROPERTY()
	UStateManager* StateManager;

	// 攻撃管理コンポーネント（各攻撃の取得・実行）
	UPROPERTY()
	UAttackController* AttackController;

	// ==== フラグ・座標 ====

	//初期位置を保存
	FVector PreviousLocation;

	// ダッシュ中フラグ
	bool bIsDashing = false;
	//スキルフラグ
	bool bHasUsedSkill = false;

	// 前フレームとカメラ位置補正用
	FVector PlayerOldLocation;

	UMoveComponent* MoveComp;

	// ==== メッシュ・コリジョン ====

	UPROPERTY()
	UPhysicsCalculator* physics;

	UPROPERTY()
	UColorControllerComponent* colorController;
};