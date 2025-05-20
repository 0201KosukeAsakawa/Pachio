#include "Attack/StompAttackStrategy.h"
#include "GameFramework/Character.h"
#include "Interface/IDamageable.h"


bool UStompAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	IDamageable* id = Cast<IDamageable>(Target);
	if (!id || !id->TakeDamage(attackData, FinalDamage))
		return false;

	ACharacter* Character = Cast<ACharacter>(Attacker);
	// 攻撃成功：Attacker（オーナー）に上方向の力を加える
	if (!Character || !id->CanBeStomped())
		return false;

	FVector LaunchVelocity = FVector(0.0f, 0.0f, 600.0f);
	Character->LaunchCharacter(LaunchVelocity, true, true);
	return true;
}