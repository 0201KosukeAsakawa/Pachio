// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/StateManager.h"
#include "StateManagerBase.generated.h"



UCLASS()
class PACHIO_API UStateManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。GreenStateManagerの初期化（デフォルト値設定）
     */
    UStateManagerComponent();

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
    bool IsStateMatch(EPlayerStateType StateTag)const;

    /**
     * @brief 現在のアクティブステートを取得
     *
     * @return 現在のステートインスタンス
     */
    inline UPlayerStateComponent* GetCurrentState()const{ return CurrentState; }

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
};