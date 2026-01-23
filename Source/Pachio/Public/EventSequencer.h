/**
 * イベントシーケンサー
 *
 * ゲーム内の演出イベントを管理・実行する中央制御システム。
 * カメラ、アクター、時間制御などを組み合わせた複雑な演出シーケンスを構築できる。
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE5Coro.h"
#include "EventSequencer.generated.h"

class UCameraHandlerComponent;
class APlayerController;
/**
 * イベントステップの種類
 */
UENUM(BlueprintType)
enum class EEventStepType : uint8
{
	/** カメラを指定位置にフォーカス */
	CameraFocus UMETA(DisplayName = "Camera Focus"),

	/** カメラを複数地点にフォーカス */
	CameraFocusMultiple UMETA(DisplayName = "Camera Focus Multiple"),

	/** カメラズーム */
	CameraZoom UMETA(DisplayName = "Camera Zoom"),

	/** カメラシェイク */
	CameraShake UMETA(DisplayName = "Camera Shake"),

	/** カメラをプレイヤーに戻す */
	CameraReturnToPlayer UMETA(DisplayName = "Camera Return To Player"),

	/** アクターに関数を呼ぶ */
	CallActorFunction UMETA(DisplayName = "Call Actor Function"),

	/** 待機 */
	Wait UMETA(DisplayName = "Wait"),

	/** ゲーム時間の一時停止 */
	PauseGame UMETA(DisplayName = "Pause Game"),

	/** ゲーム時間の再開 */
	UnpauseGame UMETA(DisplayName = "Unpause Game"),

	/** プレイヤー入力無効化 */
	DisableInput UMETA(DisplayName = "Disable Input"),

	/** プレイヤー入力有効化 */
	EnableInput UMETA(DisplayName = "Enable Input"),

	/** サウンド再生 */
	PlaySound UMETA(DisplayName = "Play Sound"),

	/** パーティクル再生 */
	SpawnParticle UMETA(DisplayName = "Spawn Particle"),

	/** ColorEventを発火（既存システムとの互換） */
	TriggerColorEvent UMETA(DisplayName = "Trigger Color Event")
};

/**
 * イベントシーケンスの1ステップ
 */
USTRUCT(BlueprintType)
struct FEventStep
{
	GENERATED_BODY()

	/** ステップの種類 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	EEventStepType StepType = EEventStepType::Wait;

	/** 対象となるアクター（CallActorFunctionなどで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** 呼び出す関数名（CallActorFunctionで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	FName FunctionName;

	/** ターゲット位置（CameraFocus、SpawnParticleで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	FVector TargetLocation = FVector::ZeroVector;

	/** 複数ターゲット位置（CameraFocusMultipleで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	TArray<FVector> TargetLocations;

	/** 継続時間（Wait、CameraFocus、CameraZoomで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	float Duration = 1.0f;

	/** 移動速度（CameraFocusで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	float MoveSpeed = 3.0f;

	/** 強度（CameraShakeで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	float Intensity = 10.0f;

	/** ターゲットFOV（CameraZoomで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	float TargetFOV = 60.0f;

	/** サウンドアセット（PlaySoundで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	TObjectPtr<USoundBase> Sound = nullptr;

	/** パーティクルシステム（SpawnParticleで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	TObjectPtr<UParticleSystem> ParticleSystem = nullptr;

	/** 移動完了を待つか（CameraFocusで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	bool bWaitForComplete = true;

	/** カメラがターゲットを向くか（CameraFocus、CameraFocusMultipleで使用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	bool bLookAtTarget = true;

	/** ColorEvent用のイベントID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	FName ColorEventID;

	/** ColorEvent用の色 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	FLinearColor ColorEventColor = FLinearColor::White;

	/** ステップの説明（エディタ用） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Step")
	FString Description;
};

/**
 * イベントシーケンスの完全な定義
 */
USTRUCT(BlueprintType)
struct FEventSequence
{
	GENERATED_BODY()

	/** シーケンス名 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	FName SequenceName;

	/** ステップのリスト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TArray<FEventStep> Steps;

	/** シーケンス開始時にゲームを一時停止するか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bPauseGameAtStart = true;

	/** シーケンス開始時にプレイヤー入力を無効化するか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bDisableInputAtStart = true;

	/** シーケンス終了時にゲームを再開するか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bUnpauseGameAtEnd = true;

	/** シーケンス終了時にプレイヤー入力を有効化するか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bEnableInputAtEnd = true;

	/** シーケンスの説明 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	FString Description;
};

/**
 * シーケンス完了時のデリゲート
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSequenceComplete, FName, SequenceName);

/**
 * イベントシーケンサー本体
 *
 * ゲーム内の演出イベントを管理・実行する。
 * Blueprintやレベルエディタで簡単に演出シーケンスを組み立てられる。
 */
UCLASS(Blueprintable, ClassGroup = (Custom))
class PACHIO_API AEventSequencer : public AActor
{
	GENERATED_BODY()

public:
	AEventSequencer();

	/**
	 * イベントシーケンスを実行
	 *
	 * @param SequenceName 実行するシーケンス名
	 * @return シーケンスが見つかったかどうか
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Sequencer")
	bool PlaySequence(FName SequenceName);

	/**
	 * 現在実行中のシーケンスを停止
	 */
	UFUNCTION(BlueprintCallable, Category = "Event Sequencer")
	void StopCurrentSequence();

	/**
	 * シーケンスが実行中かどうか
	 */
	UFUNCTION(BlueprintPure, Category = "Event Sequencer")
	bool IsPlayingSequence() const { return bIsPlaying; }

	/**
	 * 現在実行中のシーケンス名を取得
	 */
	UFUNCTION(BlueprintPure, Category = "Event Sequencer")
	FName GetCurrentSequenceName() const { return CurrentSequenceName; }

	/** シーケンス完了時のイベント */
	UPROPERTY(BlueprintAssignable, Category = "Event Sequencer")
	FOnSequenceComplete OnSequenceComplete;

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * シーケンスを実際に実行（コルーチン）
	 */
	UE5Coro::TCoroutine<> ExecuteSequence(const FEventSequence& Sequence);

	/**
	 * 単一ステップを実行
	 */
	UE5Coro::TCoroutine<> ExecuteStep(const FEventStep& Step);

	/**
	 * カメラハンドラーを取得
	 */
	UCameraHandlerComponent* GetCameraHandler();

	/**
	 * プレイヤーコントローラーを取得
	 */
	APlayerController* GetPlayerController();

private:
	/** 登録されているシーケンスのリスト */
	UPROPERTY(EditAnywhere, Category = "Sequences", meta = (TitleProperty = "SequenceName"))
	TArray<FEventSequence> Sequences;

	/** カメラハンドラーの参照 */
	UPROPERTY(EditAnywhere, Category = "References")
	TObjectPtr<UCameraHandlerComponent> CameraHandler;

	/** 現在実行中かどうか */
	bool bIsPlaying = false;

	/** 現在実行中のシーケンス名 */
	FName CurrentSequenceName;

	/** シーケンスキャンセル用 */
	bool bShouldStop = false;
};