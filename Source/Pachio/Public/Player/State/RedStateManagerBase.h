// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/State/StateManagerBase.h"
#include "RedStateManagerBase.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API URedStateManagerBase : public UStateManagerBase
{
	GENERATED_BODY()
public:
    /**
     * @brief コンストラクタ。GreenStateManagerの初期化（デフォルト値設定）
     */
    URedStateManagerBase();

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
};
