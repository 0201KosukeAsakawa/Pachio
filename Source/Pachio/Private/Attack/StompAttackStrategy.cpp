#include "Attack/StompAttackStrategy.h"
#include "GameFramework/Character.h"
#include "Interface/IDamageable.h"


bool UStompAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	IDamageable* id = Cast<IDamageable>(Target);
	if (!id || !id->TakeDamage(attackData, FinalDamage))
		return false;

	return id->CanBeStomped();
}