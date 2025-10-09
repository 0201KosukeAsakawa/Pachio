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
class UFloatingPawnMovement;
class UCharacterMovementComponent;

class UPhysicsCalculator;
class UMoveComponent;

struct FInputActionValue;
/**
 * APlayerCharacter
 * プレイヤーキャラクターの基底クラス。
 * 入力処理、ステート遷移、カメラ制御、攻撃衝突判定などの主要機能を実装。
 */


UCLASS()
class PACHIO_API APlayerCharacter : public ACharacter, public IStateControllable,
	public IControllableMover,
	public IControllableJumper, public IControllableAbility,
	public IColorModeController, public IStickAction,
	public IOptionAction
{
	GENERATED_BODY()

public:
	// コンストラクタ（コンポーネントの生成や初期値設定）
	APlayerCharacter();

protected:
	// ゲーム開始時に一度だけ呼ばれる初期化処理
	virtual void BeginPlay() override;

public:
	// 毎フレーム実行される更新処理
	virtual void Tick(float DeltaTime) override;

	// プレイヤー入力のバインディング設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	// ======================
	// ==== 入力アクション ====
	// ======================

	// カメラ位置をグリッドサイズとZバッファで設定
	UFUNCTION(BlueprintCallable)
	void SetCameraLocation(FVector2D NewGridSize, float NewZBuffa);

	// 移動入力処理の実装（IControllableMoverのオーバーライド）
	void Movement(const FInputActionValue& Value) override;

	// ジャンプ開始処理（IControllableJumperのオーバーライド）
	void Jump(const FInputActionValue& Value) override;

	// 特殊アクション（スキル発動またはダッシュ）処理（IControllableAbilityのオーバーライド）
	void Action(const FInputActionValue& Value) override;

	// 重力スケールの設定（オーバーロードあり）
	void SetGravityScale(bool bEnable);
	void SetGravityScale(const bool bEnable, const float NewGravityScale);

	// マウスホイールスクロール入力処理
	void OnMouseScroll(const FInputActionValue& Value);

	// 色変更（IColorModeControllerのオーバーライド）
	void ChangeColor(float Value) override;

	// 色の直接設定
	void SetColor(float Value);

	// カラーモードを1つ右にシフト
	void ShiftArrayRightColorMode() override;

	// カラーモードを1つ左にシフト
	void ShiftArrayLeftColorMode() override;

	// スティック移動入力処理（IStickActionのオーバーライド）
	void OnStickMove(const FInputActionValue& Value) override;

	// メニューオープン処理（IOptionActionのオーバーライド）
	void OpenMenu(const FInputActionValue& Value) override;

	// 光る対象の更新処理
	void UpdateGlowTarget();

	// カメラコンポーネントの取得（BlueprintCallable）
	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetCamera();

	// アニメーション速度の取得
	UFUNCTION(BlueprintCallable)
	FVector GetAnimVelocity() const;

	// 現在のYaw角度を取得
	UFUNCTION(BlueprintCallable)
	float GetYaw() const;

private:
	// 現在のプレイヤーステートを取得（IStateControllableの実装）
	UPlayerStateComponent* GetPlayerState() const override;

	// 状態変更処理（ステートタグによるステート遷移）
	UPlayerStateComponent* ChangeState(EPlayerStateType Tag) override;

	// ステートの初期化処理
	void InitState();

	// =======================
	// ==== 初期化関数群 =====
	// =======================

	// 物理設定の初期化（摩擦、重力など）
	void InitPhysicsSettings();

	// 入力コンポーネントの初期化
	void InitInput();

	// 視覚的設定（メッシュのアウトライン表示など）
	void InitVisualSettings();

	// =======================
	// ==== 状態・ステート処理 =
	// =======================

	// スティック入力による回転処理
	void OnStickRotate(const FVector2D& StickInput);

	// バフ状態のリセット処理
	void ResetBuff();

	// サークル処理（未詳細）
	void Circle();

private:
	// ================
	// ==== 変数 ======
	// ================

	// ジャンプ力の基本値
	UPROPERTY(EditAnywhere)
	float JumpForce = 12;

	// ジャンプ力のバフ倍率
	float JumpBuff = 1;

	// 移動速度
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 10;

	// 移動時の効果音再生用クールダウンタイマー
	float MoveSoundCooldown = 0.f;

	// 効果音再生の最小間隔（秒）
	const float MoveSoundInterval = 0.5f; // 0.5秒に1回まで再生可能

	// デフォルト重力スケール
	UPROPERTY(EditAnywhere)
	float DefaultGravityScalse = 50.0f;

	// X軸固定位置（使い所による？）
	float FixedXLocation = 0;

	// =====================
	// ==== コンポーネント ====
	// =====================

	// カメラ制御用コンポーネント
	UPROPERTY(EditAnywhere)
	UCameraHandlerComponent* CameraComponent;

	// プレイヤーステートのクラス参照（Blueprintで設定）
	UPROPERTY(EditAnywhere, Category = "State")
	TSubclassOf<UStateManager> StateManagerClass;

	// ステート管理コンポーネント（状態遷移と更新処理を担当）
	UPROPERTY()
	UStateManager* StateManager;

	// 物理計算用コンポーネント（地面判定や重力処理）
	UPROPERTY()
	UPhysicsCalculator* physics;

	// カラーゲージ管理コンポーネント（色の状態管理）
	UPROPERTY()
	UColorControllerComponent* colorController;

	// マウスの前回入力方向
	FVector2D PrevMouseDir;

	// マウス前回入力があるかフラグ
	bool bHasPrevMouse = false;

	// 前回の入力方向（ゲームパッドなど）
	FVector2D PrevInputDir = FVector2D::ZeroVector;

	// 前回の入力があるかフラグ
	bool bHasPrevInputDir = false;

	// 現在光らせている対象のActor
	UPROPERTY()
	AActor* CurrentGlowTarget;

private:
	// ============================
	// ==== 定数定義（constexpr） ====
	// ============================

	static constexpr int32 OUTLINE_STENCIL_VALUE = 10;

	static constexpr float MOUSE_COLOR_CHANGE_RATE = 0.01f;

	static constexpr float SCROLL_COLOR_CHANGE_RATE = 0.1f;

	static constexpr float STICK_DEADZONE = 0.02f;

	static constexpr float MOVE_SOUND_INTERVAL = 0.5f;

	static constexpr float MOUSE_DELTA_THRESHOLD = 4.0f;

	static constexpr float GLOW_INTENSITY_ON = 1.0f;

	static constexpr float GLOW_INTENSITY_OFF = 0.0f;
};
