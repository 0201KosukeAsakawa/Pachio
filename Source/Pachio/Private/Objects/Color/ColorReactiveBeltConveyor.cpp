#include "Objects/Color/ColorReactiveBeltConveyor.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/BoxComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"

// コンストラクタ：アクターの初期設定
AColorReactiveBeltConveyor::AColorReactiveBeltConveyor()
{
    // このアクターが毎フレーム Tick を呼び出すように設定
    PrimaryActorTick.bCanEverTick = true;

    // Box Component の作成とルートへのアタッチ
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    BoxComponent->SetupAttachment(RootComponent);

    // オーバーラップイベントのバインド（開始・終了）
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AColorReactiveBeltConveyor::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AColorReactiveBeltConveyor::OnOverlapEnd);
}

// ベルトコンベア初期化（親クラスの初期化も呼び出し）
void AColorReactiveBeltConveyor::Initialize()
{
    AColorReactiveObject::Initialize(); // 親の Init を呼ぶ
    CurrentDirection = direction; // 初期方向を設定
    CurrentPower = DefaultPower;


    //音との連携をしなくなったためコメントアウト
    /*if (!bPlayBeat)
        return;

    const TObjectPtr<USoundManager> SoundManager = Cast<USoundManager>(ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetObject());
    if (!SoundManager) return;

    SoundManager->OnBeatDetected.AddDynamic(this, &AColorReactiveBeltConveyor::OnBeatDetected);*/
}

// 毎フレーム呼ばれる処理（Tick）
void AColorReactiveBeltConveyor::Tick(float DeltaTime)
{
    if (!BoxComponent->IsCollisionEnabled())
    {
        hitObject.Empty();
        return;
    }

    if (bOnlyClosest)
    {
        FVector MyLocation = GetActorLocation();

        for (UPhysicsCalculator* target : hitObject)
        {
            if (target)
            {
                AActor* TargetActor = target->GetOwner();
                if (!TargetActor)
                    continue;

                FVector TargetLocation = TargetActor->GetActorLocation();

                // 自分と対象の間に遮蔽物があるかどうかラインテスト
                FHitResult HitResult;
                FCollisionQueryParams Params;
                Params.AddIgnoredActor(this);  // 自分自身は無視
                Params.AddIgnoredActor(TargetActor); // 対象も無視して直接見えるか判定

                bool bHit = GetWorld()->LineTraceSingleByChannel(
                    HitResult,
                    MyLocation,
                    TargetLocation,
                    ECC_Visibility,
                    Params
                );

                if (!bHit)
                {
                    // 遮蔽物無し → 力を加える
                    target->AddForce(CurrentDirection, CurrentPower, true, bUseLocalOffset);
                }
                // bHit == true なら遮蔽物があるので加えない
            }
        }
    }
    else
    {
        // これまで通り、全ての対象に力を加える
        for (UPhysicsCalculator* target : hitObject)
        {
            if (target)
            {
                target->AddForce(CurrentDirection, CurrentPower, true, bUseLocalOffset);
            }
        }
    }
}

// 指定された色に反応する処理
void AColorReactiveBeltConveyor::ApplyColorWithMatching(const FLinearColor& InColor,const FEffectMatchResult& result)
{
    //ApplyColorToMaterial(InColor);
    
    if (!ObjectColorComponent)
        return;

    AColorReactiveObject::ApplyColorWithMatching(InColor,result);

    // 色の一致状態を設定
    ObjectColorComponent->SetColorMatched(ObjectColorComponent->MatchesColorByRGB(result,InColor));
    if (ObjectColorComponent->IsColorMatched())
    {
        if (IsRevers)
        {
            // 色が一致していて IsRevers が true → 逆方向
            CurrentDirection = -direction;
        }
        else
        {
            // 色が一致していて IsRevers が false → 停止
            CurrentDirection = FVector::ZeroVector;
        }
    }
    else
    {
        // 色が一致していない → 通常方向
        CurrentDirection = direction;
    }
}

// オーバーラップ開始時の処理
void AColorReactiveBeltConveyor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == nullptr || OtherActor == this)
        return;

    // 対象アクターに UPhysicsCalculator があるか確認
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return;

    // すでにリストに追加されていなければ追加
    if (!hitObject.Contains(PhysicsCalculator))
    {
        hitObject.Add(PhysicsCalculator);
    }
}

// オーバーラップ終了時の処理
void AColorReactiveBeltConveyor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == nullptr || OtherActor == this)
        return;

    // 対象から UPhysicsCalculator を取得
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return;

    // 対象をリストから除外
    hitObject.Remove(PhysicsCalculator);
}

void AColorReactiveBeltConveyor::OnBeatDetected()
{
    //if (beatCount > playBeatCount)
    //{
    //    ++playBeatCount;
    //    return;
    //}

    //// コリジョン無効時は処理スキップ
    //if (!BoxComponent->IsCollisionEnabled())
    //{
    //    // 無効なので、force を止める（例: hitObject をリセット）
    //    hitObject.Empty();
    //    return;
    //}

    //for (UPhysicsCalculator* target : hitObject)
    //{
    //    if (target)
    //    {
    //        // 力を加える（CurrentDirection 方向に power の強さで）
    //        target->AddForce(CurrentDirection, CurrentPower, true);
    //    }
    //}

    //playBeatCount = 0;
}
