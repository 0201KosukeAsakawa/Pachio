#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateManager.generated.h"

// 前方宣言：プレイヤーの状態を管理する基底クラス
class UPlayerStateComponent;
class ACharacter;

UENUM(BlueprintType)
enum class EPlayerStateType : uint8
{
	Default     UMETA(DisplayName = "Default"),
	Hold     UMETA(DisplayName = "Hold"),
	Climb    UMETA(DisplayName = "Climb"),
	Dead     UMETA(DisplayName = "Dead"),
	// 他のステートを追加...
};

/**
 * プレイヤーの状態（ステート）を切り替えて制御するコンポーネント
 */
UCLASS(Blueprintable)
class PACHIO_API UStateManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ：デフォルト値の設定
	UStateManager();

	// ゲーム開始時の初期化処理
	void Init(APawn* Owner, UWorld* World);

	// 毎フレーム呼び出される更新処理（Tick 相当）
	void Update(float DeltaTime);

	// 状態を切り替える（タグ指定）
	UPlayerStateComponent* ChangeState(EPlayerStateType NextStateTag);

	UFUNCTION(BlueprintCallable)
	bool IsStateMatch(EPlayerStateType StateTag);

	// 現在のステートを取得
	inline UPlayerStateComponent* GetCurrentState() const { return CurrentState; }

private:
	// ステート名（文字列）とステートインスタンスのマップ
	UPROPERTY(EditAnywhere)
	TMap<EPlayerStateType, TSubclassOf<UPlayerStateComponent>> StateClassMap;

	// ステートの所有キャラクター
	UPROPERTY()
	APawn* mOwner;

	// 現在のアクティブなステート
	UPROPERTY()
	UPlayerStateComponent* CurrentState;

	// ワールドへの参照（Tick処理等で使用）
	UPROPERTY()
	UWorld* pWorld;
};