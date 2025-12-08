// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/ColorTargetTypes.h"
#include "TeleportPortal.generated.h"

/**
 * テレポートポータルを表すクラス。
 *
 * 色の影響を受けるオブジェクトであり、設定された条件に応じて
 * 他のポータルへアクターを転送する。
 */
UCLASS()
class PACHIO_API ATeleportPortal : public AColorReactiveObject
{
    GENERATED_BODY()

public:
    /** コンストラクタ：各種コンポーネントの初期化を行う */
    ATeleportPortal();

protected:
    /** ゲーム開始時の初期化処理 */
    virtual void BeginPlay() override;

    /**
     * 初期化処理（親クラスの初期化も呼び出す）
     */
    virtual void Initialize() override;

    /**
     * 指定された色が一致した際に呼ばれる処理
     *
     * @param InColor 適用する色
     */
    virtual void ApplyColorWithMatching(const FLinearColor& InColor) override;

    /**
     * アクターがポータルのコリジョン領域に侵入した際の処理
     *
     * @param OverlappedComp  オーバーラップしたコンポーネント
     * @param OtherActor      侵入してきたアクター
     * @param OtherComp       アクターのコンポーネント
     * @param OtherBodyIndex  複数コリジョン用のインデックス
     * @param bFromSweep      スウィープによる検出かどうか
     * @param SweepResult     スウィープ結果情報
     */
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    /** オーバーラップを検知するコリジョンボックス */
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;

    /** このポータルに対応する色効果 */
    UPROPERTY(EditAnywhere, Category = "Color")
    EColorCategory Effect;

    /** もう一方の色（セカンダリカラー） */
    FLinearColor SecondColor;

    /** 現在の転送先ポータル（実際にリンク中のポータル） */
    UPROPERTY()
    ATeleportPortal* CurrentTargetPortal;

    /** 通常時の転送先ポータル */
    UPROPERTY(EditAnywhere, Category = "Teleport")
    ATeleportPortal* PrimaryDestination;

    /** 別条件で転送されるポータル（色や効果によって切り替え） */
    UPROPERTY(EditAnywhere, Category = "Teleport")
    ATeleportPortal* AlternatePortal;

    /** テレポートを許可するアクタータグのリスト */
    UPROPERTY(EditAnywhere, Category = "Teleport")
    TArray<FName> AllowedTags;

    /** テレポート直後の再発動を防止するためのタイムスタンプ */
    UPROPERTY()
    TMap<AActor*, float> LastTeleportTime;

    /** テレポートのクールタイム（秒） */
    UPROPERTY(EditAnywhere, Category = "Teleport")
    float TeleportCooldown;
};

