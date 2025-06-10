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
	// コンストラクタ（コンポーネントの生成など）
	APlayerCharacter();

protected:
	// ゲーム開始時に一度だけ呼ばれる初期化処理
	virtual void BeginPlay() override;

public:
	// 毎フレーム実行される更新処理
	virtual void Tick(float DeltaTime) override;

	// 入力バインディングの初期化処理（プレイヤー操作の割り当て）
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 現在のプレイヤーステート（状態）を取得
	UPlayerStateComponent* GetPlayerState() const override;

public:
	// ======================
	// ==== 入力アクション ====
	// ======================

	// 移動入力処理
	void Movement(const FInputActionValue& Value);

	// ジャンプ開始処理
	void Jump(const FInputActionValue& Value);

	// ジャンプ終了処理
	void JumpStop(const FInputActionValue& Value);

	// 特殊アクション（スキル発動 or ダッシュ）開始処理
	void Action(const FInputActionValue& Value);

	// 特殊アクション終了処理（スキルリセット & ダッシュ終了）
	void StopAction();

	// 色ゲージの減少処理
	void DecreaseColor();

	// 色ゲージの増加処理
	void IncreaseColor();

	// カラーモードを1つ右にシフト
	void ShiftArrayRightColorMode();

	// カラーモードを1つ左にシフト
	void ShiftArrayLeftColorMode();

private:
	// ===============
	// ==== 初期化関数 ====
	// ===============

	// 移動ロジックの初期化
	void InitMovementLogic();

	// 状態(State)と攻撃コントローラの初期化
	void InitStateAndAttack();

	// 物理設定の初期化（摩擦や重力など）
	void InitPhysicsSettings();

	// 入力コンポーネントの初期化
	void InitInput();

	// 視覚的な設定（メッシュのアウトライン表示など）
	void InitVisualSettings();

	// ===============
	// ==== 状態・ステート処理 ====
	// ===============

	// 状態変更（ステートタグによる遷移）
	bool ChangeState(FString Tag) override;

	// 現在の色に応じた効果適用
	void ApplyEffectFromColor(const FLinearColor& Color);

	// ダメージ処理（ダメージ値と攻撃データを受け取る）
	bool TakeDamage(FAttackData Data, const float damage = 0, const AActor* = nullptr) override;

private:
	// =====================
	// ==== コンポーネント ====
	// =====================

	// カメラ制御コンポーネント
	UPROPERTY()
	UCameraHandlerComponent* CameraComponent;

	// 無敵状態制御コンポーネント
	UPROPERTY()
	UInvincibilityComponent* InvincibilityComponent;

	// プレイヤーのステートクラス（Blueprintから設定）
	UPROPERTY(EditAnywhere, Category = "State")
	TSubclassOf<UStateManager> StateManagerClass;

	// ステート管理コンポーネント（状態遷移・更新処理）
	UPROPERTY()
	UStateManager* StateManager;

	// 攻撃管理コンポーネント（攻撃の登録・管理・実行）
	UPROPERTY()
	UAttackController* AttackController;

	// プレイヤー移動処理を司るコンポーネント
	UPROPERTY()
	UMoveComponent* MoveComp;

	// 物理計算用コンポーネント（地面判定、重力加算など）
	UPROPERTY()
	UPhysicsCalculator* physics;

	// カラーゲージ管理コンポーネント（色状態とその変化を制御）
	UPROPERTY()
	UColorControllerComponent* colorController;

	// =====================
	// ==== プレイヤー状態 ====
	// =====================

	// ダッシュ状態のフラグ
	bool bIsDashing = false;

	// スキルを使用したかどうかのフラグ（1回制限処理用）
	bool bHasUsedSkill = false;
};
