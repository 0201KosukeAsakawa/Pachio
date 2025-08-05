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
}

void UAttackComponent::BeginPlay()
{
    Super::BeginPlay();

}

void UAttackComponent::OnAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == GetOwner())
        return;

    CurrentStrategy->ExecuteEffect(GetOwner(), OtherActor, AttackData);
    UE_LOG(LogTemp, Log, TEXT("hhh"));
}


float UAttackComponent::GetAttackPower() const
{
    // プレイヤーのレベルやバフなどを考慮する場合は、ここで補正処理を行う
    return AttackData.BaseDamage;
}

bool UAttackComponent::Init(UWorld* world, FName NewStrategy)
{
    if (!world)
        return false;

  /*  CurrentStrategy = ALevelManager::GetInstance(GetWorld())
        ->GetAttackDataContainer()
        ->CreateStrategy(world, NewStrategy);*/

    if (!CurrentStrategy)
        return false;

    AActor* Owner = GetOwner();
    if (!Owner)
        return false;

    USceneComponent* Root = Owner->GetRootComponent();
    if (!Root)
        return false;
    // 攻撃判定用のボックスコンポーネントを探す
    UBoxComponent* AttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(Owner, NewStrategy);
    if (!AttackBox)
        return false;
    // ここでアタッチ
    AttackBox->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
    // サイズ・衝突・可視化
    AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackBox->SetHiddenInGame(false);
    AttackBox->SetVisibility(true); // 念のため
    AttackBox->RegisterComponent(); // ★NewObject経由なら必要！
    // オーバーラップ設定
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