#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IDamageable.h"
#include "BaseBlock.generated.h"

class UBlockState;
class UBlockDataContainer;
class UBoxComponent;

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
	
	//�_���[�W��󂯂鏈��
	bool TakeDamage(FAttackData, float damage = 0)override;

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	//�o��������A�C�e����
	UPROPERTY(EditAnywhere)
	FString DropItemName;

	//�ŏ��̃N���X�̃^�O
	UPROPERTY(EditAnywhere)
	FString StateID;

	//�Ǘ�����X�e�[�g
	UPROPERTY()
	UBlockState* CurrentState;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> ContainerClass;

	UPROPERTY()
	TObjectPtr<UBlockDataContainer> Container;
};
