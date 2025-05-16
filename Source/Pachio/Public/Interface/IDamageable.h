#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Attack/AttackStrategy.h"

#include "IDamageable.generated.h"

// このクラスはインターフェイスとして使用されます。直接変更する必要はありません。
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * ダメージを受けることが可能なアクターが実装すべきインターフェイス
 */
class PACHIO_API IDamageable
{
	GENERATED_BODY()

public:
	/**
	 * ダメージを受ける処理を実装する関数
	 * @param Data 攻撃の詳細情報（攻撃種別や属性など）
	 * @param damage 攻撃による数値的なダメージ（省略可能、Dataからも判定可能な場合）
	 * @return true：ダメージが正常に処理された / false：何らかの理由で処理されなかった
	 */
	virtual bool TakeDamage(FAttackData Data, float damage = 0);

	/**
	 * 現在のアクターが死亡状態かどうかを返す
	 * @return true：死亡している / false：生存している
	 */
	virtual bool IsDead() const;

	virtual bool CanBeStomped() const;
};