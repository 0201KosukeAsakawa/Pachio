#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IDamageable.h"
#include "BaseBlock.generated.h"

UCLASS()
class PACHIO_API ABaseBlock : public AActor,public IDamageable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//�_���[�W���󂯂鏈��
	bool TakeDamage(FAttackData, float damage = 0)override;

	bool DropItem();
private:
	//block���ړ�������
	UPROPERTY(EditAnywhere)
	int hp;
};
