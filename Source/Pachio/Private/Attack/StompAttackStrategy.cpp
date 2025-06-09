#include "Attack/StompAttackStrategy.h"
#include "GameFramework/Character.h"
#include "Interface/IDamageable.h"
#include "Components/PhysicsCalculator.h"


bool UStompAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	if (!Attacker || !Target)
		return false;

	IDamageable* id = Cast<IDamageable>(Target);
	if (!id || !id->TakeDamage(attackData, FinalDamage))
		return false;

	if (!id->CanBeStomped())
		return false;

	UPhysicsCalculator* physics = Attacker->GetComponentByClass<UPhysicsCalculator>();
	if (!physics)
		return false;
	physics->AddForce(Attacker->GetActorUpVector(), 3);
	return true;
}