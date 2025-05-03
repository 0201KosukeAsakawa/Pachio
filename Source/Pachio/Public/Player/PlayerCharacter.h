// Fi// プレイヤーキャラクターの基本クラス
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Components/BoxComponent.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

// 前方宣言
class IStateBase;
class UPlayerDefaultState;
class UInputMappingContext;
class UStateManager;
class UAttackComponent;

UCLASS()
class PACHIO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// デフォルトコンストラクタ
	APlayerCharacter();

protected:
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void Tick(float DeltaTime) override;

	// 入力をバインド
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 上攻撃と踏みつけ攻撃の衝突判定
	UFUNCTION()
	void OnUpperAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStompAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// 状態の初期化
	void GenerateState();

private:

	// 移動・ジャンプ・アクションの入力処理
	void Movement(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void JumpStop(const FInputActionValue& Value);
	void Action(const FInputActionValue& Value);
	void StopAction();





	// 状態の変更
	void ChangeState(FString Tag);

private:
	// 状態管理クラスへのポインタ
	UPROPERTY()
	UStateManager* StateManager;

// 攻撃用のコンポーネント（上攻撃と踏みつけ）
	UPROPERTY()
	UAttackComponent* Upper;

	UPROPERTY()
	UAttackComponent* Stomp;

	// ダッシュ中かどうかのフラグ
	bool bIsDashing;

	// カメラの新しい位置とプレイヤーの以前の位置
	FVector NewCameraLocation;
	FVector PlayerOldLocation;

	

private:
	// キャラクターのカプセルメッシュ（※独自に追加？）
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Capsule;

	// 攻撃用の当たり判定ボックス
	UPROPERTY()
	UBoxComponent* UpperAttackBox;

	UPROPERTY()
	UBoxComponent* StompAttackBox;

	// カメラの回転・距離制御用のスプリングアーム
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	// プレイヤー視点用のカメラ
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	// 入力マッピングと各アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAction;
};