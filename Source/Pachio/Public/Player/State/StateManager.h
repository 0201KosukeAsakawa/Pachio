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
UCLASS()
class PACHIO_API UStateManager : public UActorComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。StateManagerの初期化（デフォルト値設定）
     */
    UStateManager();

    /**
     * @brief ゲーム開始時の初期化処理
     *
     * @param Owner このステートマネージャが管理するプレイヤーPawn
     * @param World ワールド参照
     */
    void Init(APawn* Owner, UWorld* World);

    /**
     * @brief 毎フレーム呼び出される更新処理（Tick 相当）
     *
     * @param DeltaTime 前フレームからの経過時間
     */
    void Update(float DeltaTime);

    /**
     * @brief 指定ステートタグのステートに切り替える
     *
     * @param NextStateTag 遷移先ステートのタグ
     * @return 遷移したステートインスタンス
     */
    UPlayerStateComponent* ChangeState(EPlayerStateType NextStateTag);

    /**
     * @brief 現在のステートが指定タグと一致するか確認
     *
     * @param StateTag チェックするステートタグ
     * @return 一致する場合 true
     */
    UFUNCTION(BlueprintCallable)
    bool IsStateMatch(EPlayerStateType StateTag);

    /**
     * @brief 現在のアクティブステートを取得
     *
     * @return 現在のステートインスタンス
     */
    inline UPlayerStateComponent* GetCurrentState() const { return CurrentState; }

private:
    /** @brief ステートタグとステートクラスのマップ（ステート生成用） */
    UPROPERTY(EditAnywhere)
    TMap<EPlayerStateType, TSubclassOf<UPlayerStateComponent>> StateClassMap;

    /** @brief ステートの所有キャラクター */
    UPROPERTY()
    APawn* mOwner;

    /** @brief 現在アクティブなステート */
    UPROPERTY()
    UPlayerStateComponent* CurrentState;

    /** @brief ワールドへの参照（Tick処理やステート初期化時に使用） */
    UPROPERTY()
    UWorld* pWorld;
};
