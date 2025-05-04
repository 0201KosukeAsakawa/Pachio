#include "Components/AttackManagerComponent.h"
#include "Components/AttackComponent.h"
#include "DataContainer/AttackDataContainer.h"
#include "Manager/LevelManager.h"

// コンストラクタ：このコンポーネントのデフォルト設定を初期化
UAttackManagerComponent::UAttackManagerComponent()
{

}

void UAttackManagerComponent::Init(UWorld* world)
{
	if (!world)
		return;

	pWorld = world;
}

// 攻撃コンポーネントを登録する関数
// 指定された攻撃IDと攻撃コンポーネントをマップに登録
// すでに同じIDで登録されている場合や、無効なコンポーネントが指定された場合は失敗
bool UAttackManagerComponent::RegisterAttackComponent(FName AttackID)
{
	if (!pWorld)
		return false;

	// コンポーネントが無効であるか、すでにそのIDがマップに登録されている場合は失敗
	if (AttackMap.Contains(AttackID))
	{
		return false;
	}

	const UAttackDataContainer* ad =  ALevelManager::GetInstance(pWorld)->GetAttackDataContainer();

	if (!ad)
		return false;

	UAttackComponent* AttackComponent = ad->GenerateAttackComponent(GetOwner(), AttackID);

	if (!AttackComponent)
		return false;
	 
	AttackComponent->Init(pWorld);

	if (!AttackComponent->SetAttackStrategy(AttackID))
		return false;

	// 攻撃IDと攻撃コンポーネントクラスをマップに追加
	AttackMap.Add(AttackID, AttackComponent);
	return true;
}

bool UAttackManagerComponent::ResetMap()
{
	AttackMap.Empty();
	return true;
}

// 指定された攻撃IDに対応する攻撃コンポーネントを取得
// 攻撃IDに対応するコンポーネントが登録されていない場合はnullptrを返す
 UAttackComponent* UAttackManagerComponent::GetAttack(FName AttackID)
{
	// 攻撃IDがマップに存在すればその攻撃コンポーネントクラスを返す
	if (UAttackComponent** Found = AttackMap.Find(AttackID))
	{
		return *Found;
	}
	// 見つからなかった場合はnullptrを返す
	return nullptr;
}