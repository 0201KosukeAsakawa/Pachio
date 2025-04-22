#include "Attack/StompAttackStrategy.h"
#include "Interface/IDamageable.h"


void UStompAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, float FinalDamage)
{
	//ここで各攻撃の処理をしてください
	if (IDamageable* id = Cast<IDamageable>(Target))
	{
		id->TakeDamage(attackData,FinalDamage);
	}
}
