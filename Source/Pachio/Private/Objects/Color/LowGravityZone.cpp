// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/LowGravityZone.h"
#include "Components/PhysicsCalculator.h"
#include "Components/BoxComponent.h"
#include "Components/Color/ObjectColorComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// =======================
// コンストラクタ
// =======================

// デフォルト値設定
ALowGravityZone::ALowGravityZone()
                                    :bIsActive(false)
                                    , GravityScale(DEFAULT_GRAVITY_SCALE)
                                    , JumpBuff(DEFAULT_JUMP_BUFF)
{
    PrimaryActorTick.bCanEverTick = false; // 毎フレーム更新は不要

    // BoxComponent を作成しルートに設定
    ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
    RootComponent = ZoneBox;

    // オーバーラップイベントを発生させる設定
    ZoneBox->SetGenerateOverlapEvents(true);
    ZoneBox->SetCollisionProfileName(TEXT("Trigger"));
}

// =======================
// 初期化処理
// =======================

void ALowGravityZone::Initialize()
{
    // オーバーラップ開始/終了イベント登録
    ZoneBox->OnComponentBeginOverlap.AddDynamic(this, &ALowGravityZone::OnOverlapBegin);
    ZoneBox->OnComponentEndOverlap.AddDynamic(this, &ALowGravityZone::OnOverlapEnd);

    // 宇宙空間エフェクトを初期化（最初は非表示）
    if (UniverseSystem && !UniverseEffect)
    {
        UniverseEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
            UniverseSystem,
            GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            false
        );
        // UniverseEffect->SetWorldScale3D(FVector(20.f)); // 必要ならスケール調整
        UniverseEffect->Deactivate();
    }
}

// =======================
// 色反応処理
// =======================

void ALowGravityZone::ApplyColorWithMatching(const FLinearColor& InColor)
{
    if (!ObjectColorComponent || !ZoneBox)
        return;

    // 親クラス処理呼び出し
    AColorReactiveObject::ApplyColorWithMatching(InColor);

    // 色が一致しなければ「重力を元に戻す」
    if (!UColorUtilityLibrary::IsHueSimilar(InColor,GetCurrentColor()))
    {
        for (AActor* Actor : OverlappingActors)
        {
            if (UPhysicsCalculator* PhysicsComp = Actor->FindComponentByClass<UPhysicsCalculator>())
            {
                PhysicsComp->SetGravityScale(true); // デフォルトの重力に戻す
            }
        }
        OverlappingActors.Empty();

        if (UniverseEffect)
            UniverseEffect->Deactivate();

        bIsActive = false; // ゾーン無効化フラグ
        return;
    }

    // 色一致時はエフェクトを有効化
    if (UniverseEffect)
        UniverseEffect->Activate();

    bIsActive = true;

    // Box 範囲を取得
    FVector Center = ZoneBox->GetComponentLocation();
    FVector HalfSize = ZoneBox->GetScaledBoxExtent();

    // ゾーン内にいるアクターを検知
    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Center,
        Center, // 開始と終了が同じなので静的判定
        FQuat::Identity,
        ECC_PhysicsBody,
        FCollisionShape::MakeBox(HalfSize)
    );

    if (!bHit)
        return;

    // ヒットしたアクターに低重力を適用
    for (const FHitResult& Hit : HitResults)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            if (UPhysicsCalculator* PhysicsComp = HitActor->FindComponentByClass<UPhysicsCalculator>())
            {
                PhysicsComp->SetGravityScale(true, GravityScale, JumpBuff); // 重力スケールとジャンプ補正
                OverlappingActors.Add(HitActor);
            }
        }
    }
}

// =======================
// ポストプロセスエフェクト有効化（未実装）
// =======================

void ALowGravityZone::SetPostProcessEffectEnabled(bool bEnable)
{
    // TODO: ゾーン内限定の画面演出を追加する場合に使用
}

// =======================
// オーバーラップ開始処理
// =======================

void ALowGravityZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // ゾーンが無効なら処理しない
    if (!bIsActive)
        return;

    // 自身以外のアクターに対して処理
    if (OtherActor && OtherActor != this)
    {
        if (UPhysicsCalculator* PhysicsComp = OtherActor->FindComponentByClass<UPhysicsCalculator>())
        {
            // 最初に重力を軽減して適用
            PhysicsComp->SetGravityScale(true, GravityScale, JumpBuff);
            OverlappingActors.Add(OtherActor);
        }
    }
}

// =======================
// オーバーラップ終了処理
// =======================

void ALowGravityZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // ゾーンが無効なら処理しない
    if (!bIsActive)
        return;

    // 自身以外のアクターに対して処理
    if (OtherActor && OtherActor != this)
    {
        if (UPhysicsCalculator* PhysicsComp = OtherActor->FindComponentByClass<UPhysicsCalculator>())
        {
            // 重力を通常に戻す
            PhysicsComp->SetGravityScale(true);
            OverlappingActors.Remove(OtherActor);
        }
    }
}
