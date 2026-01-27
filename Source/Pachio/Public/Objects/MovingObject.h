// MovingObject.h
#pragma once
#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "DataContainer/ColorTargetTypes.h"
#include "UE5Coro.h"
#include "MovingObject.generated.h"

class UBoxComponent;

/** イージングの種類 */
UENUM(BlueprintType)
enum class EMovementEasing : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	SmoothStep
};

/** 移動モード */
UENUM(BlueprintType)
enum class EColorMovementMode : uint8
{
	/** 色一致時のみON位置、それ以外はOFF位置 */
	Toggle UMETA(DisplayName = "Toggle (On/Off)"),

	/** 色変化をトリガーにA-B間を往復 */
	Shuttle UMETA(DisplayName = "Shuttle (A ⇔ B)")
};

/** 位置座標の種類 */
UENUM(BlueprintType)
enum class ELocationMode : uint8
{
	/** ローカル座標（初期位置からの相対） */
	Local UMETA(DisplayName = "Local (Relative)"),

	/** ワールド座標（絶対位置） */
	World UMETA(DisplayName = "World (Absolute)")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UMoveOnColorComponent : public UObjectColorComponent
{
	GENERATED_BODY()

public:
	UMoveOnColorComponent();
	virtual void Initialize() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	virtual void ActivateDirect(const FLinearColor& InColor) override;

	/**
	 * イージング関数を適用した移動
	 */
	UE5Coro::TCoroutine<> MoveWithEasingAsync(FVector Target, float Duration);

	/**
	 * イージング関数の適用
	 */
	float ApplyEasing(float Alpha) const;

	/**
	 * Toggleモードの処理
	 */
	void HandleToggleMode(const FLinearColor& InColor);

	/**
	 * Shuttleモードの処理
	 */
	void HandleShuttleMode(const FLinearColor& InColor);

	/**
	 * 自動ループ処理
	 */
	UE5Coro::TCoroutine<> AutoLoopMovement();

	UFUNCTION()
	void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/** 移動モード */
	UPROPERTY(EditAnywhere, Category = "Movement")
	EColorMovementMode MovementMode = EColorMovementMode::Toggle;

	/** 地点Aの座標モード */
	UPROPERTY(EditAnywhere, Category = "Movement|Location A", meta = (DisplayName = "Location A Mode"))
	ELocationMode LocationAMode = ELocationMode::Local;

	/** 地点A（ローカル/ワールド座標） */
	UPROPERTY(EditAnywhere, Category = "Movement|Location A", meta = (DisplayName = "Location A"))
	FVector LocationA;

	/** 地点Bの座標モード */
	UPROPERTY(EditAnywhere, Category = "Movement|Location B", meta = (DisplayName = "Location B Mode"))
	ELocationMode LocationBMode = ELocationMode::Local;

	/** 地点B（ローカル/ワールド座標） */
	UPROPERTY(EditAnywhere, Category = "Movement|Location B", meta = (DisplayName = "Location B"))
	FVector LocationB;

	/** 子オブジェクトの配列（連動移動対象） */
	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<AActor*> Child;

	/** 足場判定用のトリガー */
	UPROPERTY(EditAnywhere, Category = "Collision")
	TObjectPtr<UBoxComponent> FootTrigger;

	/** 移動にかける時間（秒） */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float MoveDuration = 1.0f;

	/** イージング関数の種類 */
	UPROPERTY(EditAnywhere, Category = "Movement")
	EMovementEasing EasingType = EMovementEasing::EaseInOut;

	/** Shuttleモード: 色変化時に逆方向に移動するか */
	UPROPERTY(EditAnywhere, Category = "Movement|Shuttle", meta = (EditCondition = "MovementMode == EColorMovementMode::Shuttle", EditConditionHides))
	bool bReverseOnColorChange = true;

	/** Shuttleモード: 自動往復するか（色変化とは無関係にループ） */
	UPROPERTY(EditAnywhere, Category = "Movement|Shuttle", meta = (EditCondition = "MovementMode == EColorMovementMode::Shuttle", EditConditionHides))
	bool bAutoLoop = false;

	/** Shuttleモード: 地点Aでの待機時間 */
	UPROPERTY(EditAnywhere, Category = "Movement|Shuttle", meta = (EditCondition = "MovementMode == EColorMovementMode::Shuttle && bAutoLoop", EditConditionHides, ClampMin = "0.0", ClampMax = "10.0"))
	float WaitTimeAtA = 1.0f;

	/** Shuttleモード: 地点Bでの待機時間 */
	UPROPERTY(EditAnywhere, Category = "Movement|Shuttle", meta = (EditCondition = "MovementMode == EColorMovementMode::Shuttle && bAutoLoop", EditConditionHides, ClampMin = "0.0", ClampMax = "10.0"))
	float WaitTimeAtB = 1.0f;

	/** 現在足場に乗っているアクターのリスト */
	TArray<TWeakObjectPtr<AActor>> AttachedActors;

	/** 初期ワールド位置 */
	FVector InitialWorldLocation;

	/** 地点Aのワールド座標（計算後） */
	FVector WorldLocationA;

	/** 地点Bのワールド座標（計算後） */
	FVector WorldLocationB;

	/** Shuttleモード: 現在の目標地点（true = B, false = A） */
	bool bCurrentTargetIsB = false;

	/** 自動ループが実行中かどうか */
	std::atomic<bool> bIsAutoLoopRunning{ false };

private:
	static constexpr float DEFAULT_DURATION = 1.0f;
	static constexpr float MIN_DURATION = 0.01f;
};