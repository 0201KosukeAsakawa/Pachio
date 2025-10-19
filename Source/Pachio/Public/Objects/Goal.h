#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Goal.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API AGoal : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。ゴールアクターの初期化を行う
     */
    AGoal();

protected:
    /**
     * @brief ゲーム開始時またはスポーン時に呼ばれる初期化処理
     */
    virtual void BeginPlay() override;

    /** @brief ゴール判定用のボックスコンポーネント */
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* GoalBox;

    /**
     * @brief ゴールエリアにアクターが接触したときに呼ばれるオーバーラップイベント
     *
     * @param OverlappedComponent オーバーラップされたコンポーネント
     * @param OtherActor 接触したアクター
     * @param OtherComponent 接触したアクターのコンポーネント
     * @param OtherBodyIndex ボディインデックス
     * @param bFromSweep スイープによる衝突かどうか
     * @param SweepResult 衝突の詳細情報
     */
    UFUNCTION()
    void OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

public:
    /**
     * @brief 毎フレーム呼び出される更新処理
     *
     * @param DeltaTime 前フレームからの経過時間
     */
    virtual void Tick(float DeltaTime) override;

    /**
     * @brief ゴールに到達したかどうかを取得する
     *
     * @return ゴール到達済みの場合 true、それ以外は false
     */
    inline bool IsGoal() const { return isGoal; }

private:
    /** @brief ゴール到達フラグ */
    bool isGoal;

    /** @brief 次のワールド（ステージ）名 */
    UPROPERTY(EditAnywhere)
    FName nextWorldName;
};
