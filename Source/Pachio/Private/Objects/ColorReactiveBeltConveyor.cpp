#include "Objects/ColorReactiveBeltConveyor.h"
#include "Components/ColorConfigurator.h"
#include "Components/PhysicsCalculator.h"
#include "Components/BoxComponent.h"
#include "Manager/LevelManager.h"

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
void AColorReactiveBeltConveyor::Init()
{
    AColorReactiveObject::Init(); // 親の Init を呼ぶ
    CurrentDirection = direction; // 初期方向を設定
}

// 毎フレーム呼ばれる処理（Tick）
void AColorReactiveBeltConveyor::Tick(float DeltaTime)
{
    // コリジョン無効時は処理スキップ
    if (!BoxComponent->IsCollisionEnabled())
    {
        // 無効なので、force を止める（例: hitObject をリセット）
        hitObject.Empty(); // 全て削除するならこれが最も明確で安全
        return;
    }

    for (UPhysicsCalculator* target : hitObject)
    {
        if (target)
        {
            // 力を加える（CurrentDirection 方向に power の強さで）
            target->AddForce(CurrentDirection, power, true);
        }
    }
}

// 指定された色に反応する処理
void AColorReactiveBeltConveyor::ColorAction(const FLinearColor InColor)
{
    ApplyColorToMaterial(InColor);

    if (!ColorConfigurator)
        return;

    AColorReactiveObject::ColorAction(InColor);

    // 色の一致状態を設定
    ColorConfigurator->SetColorMuch(ColorConfigurator->CheckColorMuch(InColor));

    if (ColorConfigurator->IsColorMuch())
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
    if (OtherActor == nullptr)
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
    if (OtherActor == nullptr)
        return;

    // 対象から UPhysicsCalculator を取得
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return;

    // 対象をリストから除外
    hitObject.Remove(PhysicsCalculator);
}
