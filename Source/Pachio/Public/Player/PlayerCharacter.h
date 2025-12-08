// プレイヤーキャラクターの基本クラス
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StateControllable.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "PlayerCharacter.generated.h"

// ===========================
// 前方宣言
// ===========================
class IStateBase;
class IMoveLogic;
class IStateManager;
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

class UNiagaraSystem;

struct FInputActionValue;
/**
 * APlayerCharacter
 * プレイヤーキャラクターの基底クラス。
 * 入力処理、ステート遷移、カメラ制御、攻撃衝突判定などの主要機能を実装。
 */


UCLASS()
/**
 * @brief プレイヤーキャラクタークラス
 * IStateControllable, 移動・ジャンプ・スキル・色変更など複数のインターフェースを実装
 * コンポーネント指向でステート管理、物理計算、カラー管理、入力処理を担当
 */
    class PACHIO_API APlayerCharacter : public ACharacter, public IStateControllable,
    public IControllableMover,
    public IControllableJumper, public IControllableAbility,
    public IColorModeController, public IStickAction,
    public IOptionAction
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ
     * コンポーネント生成や初期値設定を行う
     */
    APlayerCharacter();

protected:
    /**
     * @brief ゲーム開始時の初期化処理
     * BeginPlay内で呼ばれる
     */
    virtual void BeginPlay() override;

public:
    /**
     * @brief 毎フレーム更新処理
     * @param DeltaTime 前フレームからの経過時間
     */
    virtual void Tick(float DeltaTime) override;
    
    /**
     * @brief プレイヤー入力バインディング設定
     * @param PlayerInputComponent 入力コンポーネント
     */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    // ======================
    // ==== 入力アクション ====
    // ======================

    /**
     * @brief カメラ位置をグリッドサイズとZバッファで設定
     * @param NewGridSize カメラグリッドサイズ
     * @param NewZBuffa カメラZバッファ位置
     */
    UFUNCTION(BlueprintCallable)
    void SetCameraLocation(FVector2D NewGridSize, float NewZBuffa);

    /**
     * @brief 移動入力処理（IControllableMover実装）
     * @param Value 入力値（軸方向やスティック情報を含む）
     */
    void Movement(const FInputActionValue& Value) override;

    /**
     * @brief ジャンプ入力処理（IControllableJumper実装）
     * @param Value 入力値（ジャンプボタン押下情報など）
     */
    void Jump(const FInputActionValue& Value) override;

    /**
     * @brief 特殊アクション（スキル/ダッシュ）入力処理（IControllableAbility実装）
     * @param Value 入力値（ボタン押下やスティック情報を含む）
     */
    void Action(const FInputActionValue& Value) override;

    /**
     * @brief 重力スケールの有効/無効設定
     * @param bEnable 有効化するか無効化するか
     */
    void SetGravityScale(bool bEnable);

    /**
     * @brief 重力スケールの設定（スケール値指定あり）
     * @param bEnable 有効化するか無効化するか
     * @param NewGravityScale 新しい重力スケール値
     */
    void SetGravityScale(const bool bEnable, const float NewGravityScale);

    /**
     * @brief マウスホイールスクロール入力処理
     * @param Value 入力値（スクロール方向や量）
     */
    void OnMouseScroll(const FInputActionValue& Value);

    /**
     * @brief 色変更（IColorModeController実装）
     * @param Value 色の変化量（正/負で色モードを切り替え）
     */
    void ChangeColor(float Value) override;

    /**
     * @brief 色の直接設定
     * @param Value 設定する色の値（0-1など正規化値）
     */
    void SetColor(float Value);

    /**
     * @brief カラーモードを右にシフト（IColorModeController実装）
     */
    void ChangeCameraViewModeToCharacter() override;

    /**
     * @brief カラーモードを左にシフト（IColorModeController実装）
     */
    void ChangeCameraViewModeToGrid() override;

    /**
     * @brief スティック移動入力処理（IStickAction実装）
     * @param Value 入力値（2Dスティック方向ベクトル）
     */
    void OnStickMove(const FInputActionValue& Value) override;

    /**
     * @brief メニューオープン処理（IOptionAction実装）
     * @param Value 入力値（メニュー開閉ボタンの押下状態）
     */
    void OpenMenu(const FInputActionValue& Value) override;

    /**
     * @brief 現在光らせている対象を更新
     */
    void UpdateGlowTarget();

    /**
     * @brief カメラコンポーネント取得
     * @return UCameraComponent* 現在のカメラコンポーネント
     */
    UFUNCTION(BlueprintCallable)
    UCameraComponent* GetCamera()const;

    /**
     * @brief アニメーション速度取得
     * @return FVector 現在の移動速度ベクトル
     */
    UFUNCTION(BlueprintCallable)
    FVector GetAnimVelocity() const;

    /**
     * @brief 現在のYaw角度取得
     * @return float Yaw角度
     */
    UFUNCTION(BlueprintCallable)
    float GetYaw() const;

    UFUNCTION(BlueprintCallable)
    void ApplayColorToEffect(FLinearColor NewColor);

private:
    // ============================
    // ==== ステート管理関連 ======
    // ============================

    /** @brief 現在のプレイヤーステート取得（IStateControllable実装） */
    UPlayerStateComponent* GetPlayerState() const override;

    /** @brief ステート変更処理（タグ指定） */
    UPlayerStateComponent* ChangeState(EPlayerStateType Tag) override;

    // ============================
    // ==== 初期化関数群 ==========
    // ============================

    /** @brief 物理設定の初期化（摩擦、重力など） */
    void InitPhysicsSettings();

    /** @brief 入力初期化 */
    void InitInput();

    /** @brief 視覚設定（メッシュアウトラインなど） */
    void InitVisualSettings();

    // ============================
    // ==== 状態・ステート処理 ===
    // ============================

    /** @brief スティック入力による回転処理
     * @param StickInput 2Dスティック入力値
     */
    void OnStickRotate(const FVector2D& StickInput);

    /** @brief バフ状態リセット */
    void ResetBuff();

    /** @brief サークル処理（未詳細） */
    void Circle();

private:
    // ============================
    // ==== プレイヤー変数 ========
    // ============================

    /** ジャンプ力の基本値 */
    UPROPERTY(EditAnywhere)
    float JumpForce;

    /** ジャンプ力バフ倍率 */
    float JumpBuff;

    /** 移動速度 */
    UPROPERTY(EditAnywhere)
    float MoveSpeed;

    /** 移動時効果音再生用クールダウン */
    float MoveSoundCooldown;

    /** デフォルト重力スケール */
    UPROPERTY(EditAnywhere)
    float DefaultGravityScales;

    /** X軸固定位置 */
    float FixedXLocation;

    // ============================
    // ==== コンポーネント ========
    // ============================

    /** カメラ制御用コンポーネント */
    UPROPERTY(EditAnywhere)
    UCameraHandlerComponent* CameraHandleComponent;

    // 実際に生成したステートマネージャーのUObject保持
    UPROPERTY(EditAnywhere)
    UStateManagerComponent* StateManagerComponent;

    /** 物理計算用コンポーネント（地面判定や重力処理） */
    UPROPERTY()
    UPhysicsCalculator* physics;

    /** カラーゲージ管理コンポーネント */
    UPROPERTY()
    UColorControllerComponent* colorController;

    /** 前回マウス入力方向 */
    FVector2D PrevMouseDir;

    /** 前回マウス入力フラグ */
    bool bHasPrevMouse;

    /** 前回入力方向（ゲームパッド） */
    FVector2D PrevInputDir;

    /** 前回入力有無フラグ */
    bool bHasPrevInputDir;

    /** 現在光らせている対象Actor */
    UPROPERTY()
    AActor* CurrentGlowTarget;

    UPROPERTY(EditAnywhere)
    class UNiagaraComponent* water;
};
