// プレイヤーキャラクターの基本クラス
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AttackController.h"
#include "Interface/IDamageable.h"
#include "Interface/StateControllable.h"
#include "PlayerCharacter.generated.h"

// ===========================
// 前方宣言
// ===========================
class IStateBase;
class UPlayerDefaultState;
class UInputMappingContext;
class UStateManager;
class UAttackComponent;
class UAttackManagerComponent;
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UInputAction;

struct FInputActionValue;
/**
 * APlayerCharacter
 * プレイヤーキャラクターの基底クラス。
 * 入力処理、ステート遷移、カメラ制御、攻撃衝突判定などの主要機能を実装。
 */
UCLASS()
class PACHIO_API APlayerCharacter : public ACharacter, public IAttackController,public IStateControllable,public IDamageable
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

	// 攻撃戦略の登録
	virtual bool AssignAttackStrategy(FName AttackID, UAttackStrategy* NewStrategy) override;

	//プレイヤー変身時の当たり判定の変更
	void PowerUpCollisionPosition();
	void PowerDownCollisionPosition();

private:
	// ==== 入力アクション ====
	// 移動入力
	void Movement(const FInputActionValue& Value);

	// ジャンプ開始・終了
	void Jump(const FInputActionValue& Value);
	void JumpStop(const FInputActionValue& Value);

	// ダッシュ（特殊アクション）開始・終了
	void Action(const FInputActionValue& Value);
	void StopAction();

	// ==== 攻撃コリジョン ====

	// 上攻撃ヒット処理
	UFUNCTION()
	void OnUpperAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 踏みつけ攻撃ヒット処理
	UFUNCTION()
	void OnStompAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ステートの変更（タグ指定）
	bool ChangeState(FString Tag)override;
	bool TakeDamage(FAttackData Data, float damage = 0)override;

private:
	// ==== 状態・戦闘 ====

		/** カメラのY座標最大値（右スクロール限界用） */
	float MaxCameraY;

	FVector CameraXZ;

	//ステート管理のクラス
	UPROPERTY(EditAnywhere, Category = "State")
	TSubclassOf<UStateManager> StateManagerClass;

	// 状態管理コンポーネント
	UPROPERTY()
	UStateManager* StateManager;

	// 攻撃管理コンポーネント（各攻撃の取得・実行）
	UPROPERTY()
	UAttackManagerComponent* AttackManager;

	// ==== フラグ・座標 ====

	// ダッシュ中フラグ
	bool bIsDashing;

	//無敵時間中フラグ
	bool bIsInvincible = false;
	float InvincibleTime = 0.0f; //無敵時間の計測タイマー
	float MaxInvincibleTime = 2.0f; // 無敵時間を2秒と仮定

	// 前フレームとカメラ位置補正用
	FVector PlayerOldLocation;

	// ==== メッシュ・コリジョン ====

	// キャラクター用のカプセルメッシュ（カスタム追加の場合）
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Capsule;

	// 上攻撃用の当たり判定
	UPROPERTY()
	UBoxComponent* UpperAttackBox;

	// 踏みつけ攻撃用の当たり判定
	UPROPERTY()
	UBoxComponent* StompAttackBox;

	/*調整用の当たり判定
	UPROPERTY()
	UBoxComponent* PlayerBoxCollision;*/

	// ==== カメラ ====

	// カメラの回転／位置制御用スプリングアーム
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	// プレイヤー視点用カメラ
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	// ==== 入力マッピング ====

	// 使用する入力マッピングコンテキスト（Enhanced Input）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// 各種アクション設定
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAction;
};