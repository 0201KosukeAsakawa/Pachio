#include "Components/AttackComponent.h"
#include "Components/BoxComponent.h"
#include "Attack/AttackStrategy.h"
#include "Manager/LevelManager.h"
#include "DataContainer/AttackDataContainer.h"
#include "FunctionLibrary.h"

UAttackComponent::UAttackComponent()
{
    // このコンポーネントはTickを使わない（毎フレームの更新は不要）
    PrimaryComponentTick.bCanEverTick = false;

    // 攻撃判定用のボックスコンポーネントを探す
    AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
}

void UAttackComponent::BeginPlay()
{
    Super::BeginPlay();

}

void UAttackComponent::OnAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}


float UAttackComponent::GetAttackPower() const
{
    // プレイヤーのレベルやバフなどを考慮する場合は、ここで補正処理を行う
    return AttackData.BaseDamage;
}

// Init内のRegisterComponentは削除してOK

bool UAttackComponent::Init(UWorld* world, FName NewStrategy)
{
    if (!world)
        return false;

    CurrentStrategy = ALevelManager::GetInstance(GetWorld())
        ->GetAttackDataContainer()
        ->CreateStrategy(world, NewStrategy);

    if (!CurrentStrategy)
        return false;

    if (!AttackBox)
        return false;

    AActor* Owner = GetOwner();
    if (!Owner)
        return false;

    USceneComponent* OwnerRoot = Owner->GetRootComponent();
    if (!OwnerRoot)
        return false;

    // アタッチ先設定（重要）
    AttackBox->SetupAttachment(OwnerRoot);

    // 大きさは必ず指定する（例）
    AttackBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));

    // 衝突の有効化（必要に応じて調整）
    AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // デバッグのため見えるようにしておく
    AttackBox->SetHiddenInGame(false);

    AttackBox->OnComponentBeginOverlap.AddDynamic(this, &UAttackComponent::OnAttack);

    UE_LOG(LogTemp, Log, TEXT("AttackComponent Init success on owner: %s"), *Owner->GetName());

    return true;
}



void UAttackComponent::SetAttackData(EAttackType type, EBreakLevel level,const float damage,const bool projectile,const bool destroy,const UParticleSystem* effect)
{
    AttackData.attackType = type;
    AttackData.breakLevel = level;
    AttackData.BaseDamage = damage;
    AttackData.bIsProjectile = projectile;
    AttackData.bDestroyAfterHit = destroy;
    AttackData.AttackEffect = effect;
}

const bool UAttackComponent::PerformAttack(AActor* Target)
{
    // 現在の攻撃戦略が存在し、対象が有効な場合に攻撃を実行
    if (CurrentStrategy && Target)
    {
        // 戦略に応じた攻撃効果を実行（ノックバックやエフェクトも含めて処理）
        return CurrentStrategy->ExecuteEffect(GetOwner(), Target, AttackData, AttackData.BaseDamage);
    }
    return false;
}