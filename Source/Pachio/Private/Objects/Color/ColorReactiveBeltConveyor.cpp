#include "Objects/Color/ColorReactiveBeltConveyor.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/BoxComponent.h"
#include "ColorUtilityLibrary.h"
#include "DataContainer/ColorTargetTypes.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"

// ====================================================================
// コンストラクタ：ベルトコンベアの基本設定
// ====================================================================
UColorReactiveBeltConveyorComponent::UColorReactiveBeltConveyorComponent()
    : BaseDirection(1.f, 0.f, 0.f)               // デフォルト進行方向（X軸）
    , DefaultPower(100.0f)                       // 基本推進力
    , CurrentDirection(FVector::ZeroVector)      // 現在の方向（初期はゼロ）
    , bReverseOnComplementary(false)             // 色反転OFF
    , bUseLocalOffset(false)                     // ローカルオフセットOFF
    , bAffectOnlyClosest(false)                  // 近い対象のみOFF
{
    // BoxComponent（当たり判定）を生成
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    BoxComponent->InitBoxExtent(FVector(100.f, 100.f, 50.f)); // デフォルトサイズ設定
    BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
    BoxComponent->SetGenerateOverlapEvents(true);
    BoxComponent->SetupAttachment(this);

    // 衝突イベントを安全にバインド
    if (!BoxComponent->OnComponentBeginOverlap.IsAlreadyBound(this, &UColorReactiveBeltConveyorComponent::OnOverlapBegin))
    {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(
            this,
            &UColorReactiveBeltConveyorComponent::OnOverlapBegin
        );
    }

    if (!BoxComponent->OnComponentEndOverlap.IsAlreadyBound(this, &UColorReactiveBeltConveyorComponent::OnOverlapEnd))
    {
        BoxComponent->OnComponentEndOverlap.AddDynamic(
            this,
            &UColorReactiveBeltConveyorComponent::OnOverlapEnd
        );
    }

    // Tickを有効化
    PrimaryComponentTick.bCanEverTick = true;
}

// ベルトコンベア初期化（親クラスの初期化も呼び出し）
void UColorReactiveBeltConveyorComponent::Initialize()
{
    UObjectColorComponent::Initialize(); // 親の Init を呼ぶ
    CurrentDirection = BaseDirection; // 初期方向を設定

}

void UColorReactiveBeltConveyorComponent::OnRegister()
{
    UObjectColorComponent::OnRegister();
}

// 毎フレーム呼ばれる処理（Tick）
void UColorReactiveBeltConveyorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!BoxComponent->IsCollisionEnabled())
    {
        HitObjects.Empty();
        return;
    }

    if (bAffectOnlyClosest)
    {
        FVector MyLocation = GetOwner()->GetActorLocation();

        for (UPhysicsCalculator* target : HitObjects)
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
                Params.AddIgnoredActor(GetOwner());  // 自分自身は無視
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
                    target->AddForce(CurrentDirection, DefaultPower, true, bUseLocalOffset);
                }
                // bHit == true なら遮蔽物があるので加えない
            }
        }
    }
    else
    {
        // これまで通り、全ての対象に力を加える
        for (UPhysicsCalculator* target : HitObjects)
        {
            if (target)
            {
                target->AddForce(CurrentDirection, DefaultPower, true, bUseLocalOffset);
            }
        }
    }
}

// 指定された色に反応する処理
void UColorReactiveBeltConveyorComponent::ApplyColorWithMatching(const FLinearColor& InColor)
{
    SetColor(InColor);

    // 色の一致状態を設定
   SetColorMatched(UColorUtilityLibrary::IsHueSimilar(GetCurrentColor(), InColor));
    if (IsColorMatched())
    {
        if (bReverseOnComplementary)
        {
            // 色が一致していて IsRevers が true → 逆方向
            CurrentDirection = -BaseDirection;
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
        CurrentDirection = BaseDirection;
    }
}

// オーバーラップ開始時の処理
void UColorReactiveBeltConveyorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == nullptr || OtherActor == GetOwner())
        return;

    // 対象アクターに UPhysicsCalculator があるか確認
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return;

    // すでにリストに追加されていなければ追加
    if (!HitObjects.Contains(PhysicsCalculator))
    {
        HitObjects.Add(PhysicsCalculator);
    }
}

// オーバーラップ終了時の処理
void UColorReactiveBeltConveyorComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == nullptr || OtherActor == GetOwner())
        return;

    // 対象から UPhysicsCalculator を取得
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return;

    // 対象をリストから除外
    HitObjects.Remove(PhysicsCalculator);
}

void UColorReactiveBeltConveyorComponent::OnBeatDetected()
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
