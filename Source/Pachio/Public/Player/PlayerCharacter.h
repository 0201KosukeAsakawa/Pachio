// プレイヤーキャラクターの基本クラス
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StateControllable.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "Player/State/StateManager.h"
#include "PlayerCharacter.generated.h"

// ===========================
// 前方宣言
// ===========================
class IStateBase;
class IMoveLogic;
class UPlayerDefaultState;
class UInputMappingContext;
class UInputAction;
class UAttackComponent;
class UAttackController;
class USpringArmComponent;
class UCameraComponent;
class UColorControllerComponent;
class UBoxComponent;
class UCameraHandlerComponent;
class UInvincibilityComponent;
class UFloatingPawnMovement;

class UPhysicsCalculator;
class UMoveComponent;

struct FInputActionValue;
/**
 * APlayerCharacter
 * プレイヤーキャラクターの基底クラス。
 * 入力処理、ステート遷移、カメラ制御、攻撃衝突判定などの主要機能を実装。
 */
UCLASS()
class PACHIO_API APlayerCharacter : public APawn, public IStateControllable,
	public IControllableMover,
	public IControllableJumper, public IControllableAbility,
	public IColorModeController, public IStickAction,
	public IOptionAction
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


public:

	// ======================
	// ==== 入力アクション ====
	// ======================
	UFUNCTION(BlueprintCallable)
	void SetCameraLocation(FVector2D NewGridSize, float NewZBuffa);

	// 移動入力処理
	void Movement(const FInputActionValue& Value)override;

	// ジャンプ開始処理
	void Jump(const FInputActionValue& Value)override;

	// 特殊アクション（スキル発動 or ダッシュ）開始処理
	void Action(const FInputActionValue& Value)override;

	void SetGravityScale(bool);
	void SetGravityScale(const bool,const float);

	void OnMouseScroll(const FInputActionValue& Value);

	void ChangeColor(float)override;
	void SetColor(float);

	// カラーモードを1つ右にシフト
	void ShiftArrayRightColorMode()override;

	// カラーモードを1つ左にシフト
	void ShiftArrayLeftColorMode()override;

	void OnStickMove(const FInputActionValue& Value)override;
	void CallOnClosestOverlappingActor();

	void OpenMenu(const FInputActionValue& Value)override;

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetCamera();

	UFUNCTION(BlueprintCallable)
	FVector GetAnimVelocity() const;

	UFUNCTION(BlueprintCallable)
	float GetYaw()const;
	void UpdateGlowTarget();
	void Respawn();
	UFUNCTION(BlueprintCallable)
	void UpdateRespawn(FVector newLocation);
private:
	// 現在のプレイヤーステート（状態）を取得
	UPlayerStateComponent* GetPlayerState() const override;

	// 状態変更（ステートタグによる遷移）
	UPlayerStateComponent* ChangeState(EPlayerStateType Tag) override;
	void InitState();
	// ===============
	// ==== 初期化関数 ====
	// ===============

	// 物理設定の初期化（摩擦や重力など）
	void InitPhysicsSettings();

	// 入力コンポーネントの初期化
	void InitInput();

	// 視覚的な設定（メッシュのアウトライン表示など）
	void InitVisualSettings();

	// ===============
	// ==== 状態・ステート処理 ====
	// ===============

	// 現在の色に応じた効果適用
	void ApplyEffectFromColor(const FLinearColor& Color);

	void OnStickRotate(const FVector2D& StickInput);

	void ResetBuff();

	void Circle();

private:
	UPROPERTY(EditAnywhere)
	float JumpForce = 12;

	float JumpBuff = 1;
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 10;

	float MoveSoundCooldown = 0.f;
	const float MoveSoundInterval = 0.5f; // 0.5秒に1回まで再生可能

	UPROPERTY(EditAnywhere)
	float DefaultGravityScalse = 50.0f;

	float FixedXLocation = 0;
	// =====================
	// ==== コンポーネント ====
	// =====================

	// カメラ制御コンポーネント
	UPROPERTY(EditAnywhere)
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

	// 物理計算用コンポーネント（地面判定、重力加算など）
	UPROPERTY()
	UPhysicsCalculator* physics;

	// カラーゲージ管理コンポーネント（色状態とその変化を制御）
	UPROPERTY()
	UColorControllerComponent* colorController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InteractionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FloatingPawnMovement", meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* FloatingPawnMovement;

	FVector2D PrevMouseDir;
	bool bHasPrevMouse = false;

	FVector2D PrevInputDir = FVector2D::ZeroVector;
	bool bHasPrevInputDir = false;

	UPROPERTY()
	AActor* CurrentGlowTarget;

	UPROPERTY()
	FVector CurrentRespawnPoint;
};
