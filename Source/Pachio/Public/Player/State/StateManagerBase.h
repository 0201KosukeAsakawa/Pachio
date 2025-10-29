// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/StateManager.h"
#include "StateManagerBase.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) , DefaultToInstanced, BlueprintType)
class PACHIO_API UStateManagerBase : public UActorComponent, public IStateManager
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。GreenStateManagerの初期化（デフォルト値設定）
     */
    UStateManagerBase();

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
    UPlayerStateComponent* ChangeState_Implementation(EPlayerStateType NextStateTag)final override;

    /**
     * @brief 現在のステートが指定タグと一致するか確認
     *
     * @param StateTag チェックするステートタグ
     * @return 一致する場合 true
     */
    bool IsStateMatch_Implementation(EPlayerStateType StateTag) final override;

    /**
     * @brief 現在のアクティブステートを取得
     *
     * @return 現在のステートインスタンス
     */
    inline UPlayerStateComponent* GetCurrentState_Implementation()const final override { return CurrentState; }

protected:
    /** @brief ステートタグとステートクラスのマップ（ステート生成用） */
    UPROPERTY(EditAnywhere)
    TMap<EPlayerStateType, TSubclassOf<UPlayerStateComponent>> StateClassMap;

    /** @brief 現在アクティブなステート */
    UPROPERTY()
    UPlayerStateComponent* CurrentState;

    /*
    * 
    　@brief 所有元のオーナー 
    *
    * @note
    * GetOwnerをするとControllerクラスが帰ってくるため
    */
    UPROPERTY()
    APawn* mOwner;

    /*
    *
　    @brief 状態ごとの姿
    */
    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    TObjectPtr<USkeletalMesh> FormMesh;


    /*
    *
　    @brief メッシュのアニメーション
    */
    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    TSubclassOf<UAnimInstance> FormAnimBP;
};