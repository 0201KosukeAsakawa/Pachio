
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "ColorCleaner.generated.h"

/**
 * 
 */
 // ============================================================================
 // AColorCleaner
 // 敵キャラクター「ColorCleaner」
 // 一定範囲内を徘徊し、プレイヤーや特定ターゲットに接近して行動する敵AIクラス
 // ============================================================================

/**
 * @brief 色を浄化（クリア）する敵キャラクタークラス
 *
 * 一定範囲を徘徊（Wander）し、ターゲットが見つかると接近する行動を行う。
 * 段差検出（IsCliffAhead）や行動範囲制限（IsInsideMoveRange）などの
 * 簡易的なAI制御を内包している。
 */
UCLASS()
class PACHIO_API AColorCleaner : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    /**
     * @brief デフォルトコンストラクタ
     *
     * キャラクターの初期設定を行う。
     */
    AColorCleaner();

    /**
     * @brief 毎フレームの更新処理
     *
     * 行動ロジック（徘徊・ターゲット追跡・回転など）を制御する。
     *
     * @param DeltaTime 経過時間（1フレームの時間）
     */
    virtual void Tick(float DeltaTime) override;

    /**
     * @brief ゲーム開始時に呼ばれる初期化処理
     *
     * 初期位置の記録やAI初期化などを行う。
     */
    virtual void BeginPlay() override;

    /**
     * @brief 敵キャラクターの初期化処理
     *
     * 継承元の `AEnemyCharacter` から呼ばれ、個別の初期設定を行う。
     */
    virtual void Init() override;

    /**
     * @brief オーバーラップイベント時の処理
     *
     * 他アクターとの接触を検出し、条件に応じて行動を変更する。
     *
     * @param OtherActor 接触したアクター
     */
    void Overlap(AActor* OtherActor) override;

private:
    // =========================================================
    // 内部ロジック関数
    // =========================================================

    /**
     * @brief ターゲットを探索する
     *
     * 周囲のアクターからプレイヤーなどのターゲット候補を探す。
     *
     * @return 発見したターゲット（存在しない場合は nullptr）
     */
    AActor* FindTarget();

    /**
     * @brief ランダムな方向へ徘徊する処理
     *
     * 一定時間ごとに方向を変えながら移動する。
     *
     * @param DeltaTime 経過時間
     */
    void Wander(float DeltaTime);

    /**
     * @brief 指定地点へ移動する処理
     *
     * 現在位置から目標座標に向けてキャラクターを移動させる。
     *
     * @param Destination 目的地のワールド座標
     * @param DeltaTime 経過時間
     */
    void MoveTowards(const FVector& Destination, float DeltaTime);

    /**
     * @brief 指定座標が移動可能範囲内かを判定する
     *
     * 初期位置を基準とした範囲チェックを行う。
     *
     * @param Point 判定対象座標
     * @return 範囲内なら true
     */
    bool IsInsideMoveRange(const FVector& Point) const;

    /**
     * @brief 移動方向に崖があるかを検出する
     *
     * 移動先が落下領域かどうかを確認し、危険なら移動を中断する。
     *
     * @param Direction 移動方向ベクトル
     * @return 崖がある場合は true
     */
    bool IsCliffAhead(const FVector& Direction);

private:
    // =========================================================
    // アクター参照系
    // =========================================================

    /** 現在のターゲット（プレイヤーなど） */
    UPROPERTY()
    AActor* TargetActor = nullptr;

    // =========================================================
    // ベクトル関連
    // =========================================================

    /** 初期位置（移動範囲の中心となる座標） */
    FVector StartLocation;

    /** 現在の徘徊方向ベクトル */
    FVector WanderDirection = FVector::ZeroVector;

    /** 次の移動目的地（回転後に使用） */
    FVector PendingMoveDestination;

    /** 移動可能範囲の最小値（初期位置からの相対オフセット） */
    UPROPERTY(EditAnywhere, Category = "Search")
    FVector SearchAreaMin = FVector(-500.f, -500.f, 0.f);

    /** 移動可能範囲の最大値（初期位置からの相対オフセット） */
    UPROPERTY(EditAnywhere, Category = "Search")
    FVector SearchAreaMax = FVector(500.f, 500.f, 200.f);

    // =========================================================
    // 回転・行動制御関連
    // =========================================================

    /** 現在のターゲット回転値 */
    FRotator TargetRotation;

    /** 回転にかける時間（秒） */
    UPROPERTY(EditAnywhere, Category = "Movement")
    float RotationDuration = 0.5f;

    /** 回転経過時間（現在進行度） */
    float RotationElapsed = 0.f;

    /** 次の移動を待機中かどうか */
    bool bHasPendingMove = false;

    /** 現在回転中かどうか */
    bool bIsRotating = false;

    /** 現在アイドル状態（停止中）かどうか */
    bool bIsIdle = true;

    /** 次の徘徊行動までのクールダウン時間 */
    float WanderCooldown = 0.f;
};
