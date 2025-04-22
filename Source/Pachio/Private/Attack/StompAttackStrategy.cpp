#include "Attack/StompAttackStrategy.h"
#include "Interface/IDamageable.h"


void UStompAttackStrategy::ExecuteEffect(AActor* InstigatorActor, AActor* TargetActor, FAttackData type ,float FinalDamage)
{
	//ここで各攻撃の処理をしてください
	if (IDamageable* id = Cast<IDamageable>(TargetActor))
	{
		id->TakeDamage(type,FinalDamage);
	}
}
