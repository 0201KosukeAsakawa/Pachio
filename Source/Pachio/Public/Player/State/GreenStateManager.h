#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/StateManager.h"
#include "GreenStateManager.generated.h"

// 前方宣言：プレイヤーの状態を管理する基底クラス
class UPlayerStateComponent;
class ACharacter;

/**
 * プレイヤーの状態（ステート）を切り替えて制御するコンポーネント
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UGreenStateManager : public UActorComponent,public IStateManager
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。GreenStateManagerの初期化（デフォルト値設定）
     */
    UGreenStateManager();

    /**
     * @brief ゲーム開始時の初期化処理
     *
     * @param Owner このステートマネージャが管理するプレイヤーPawn
     * @param World ワールド参照
     */
    void Init_Implementation(APawn* Owner, UWorld* World)override;

    /**
     * @brief 毎フレーム呼び出される更新処理（Tick 相当）
     *
     * @param DeltaTime 前フレームからの経過時間
     */
    void Update_Implementation(float DeltaTime)override;

    /**
     * @brief 指定ステートタグのステートに切り替える
     *
     * @param NextStateTag 遷移先ステートのタグ
     * @return 遷移したステートインスタンス
     */
    UPlayerStateComponent* ChangeState_Implementation(EPlayerStateType NextStateTag)override;

    /**
     * @brief 現在のステートが指定タグと一致するか確認
     *
     * @param StateTag チェックするステートタグ
     * @return 一致する場合 true
     */
    bool IsStateMatch_Implementation(EPlayerStateType StateTag)override;

    /**
     * @brief 現在のアクティブステートを取得
     *
     * @return 現在のステートインスタンス
     */
    inline UPlayerStateComponent* GetCurrentState_Implementation() const override { return CurrentState; }

private:
    /** @brief ステートタグとステートクラスのマップ（ステート生成用） */
    UPROPERTY(EditAnywhere)
    TMap<EPlayerStateType, TSubclassOf<UPlayerStateComponent>> StateClassMap;

    /** @brief 現在アクティブなステート */
    UPROPERTY()
    UPlayerStateComponent* CurrentState;

};
