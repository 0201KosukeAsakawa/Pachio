#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IDamageable.h"
#include "BaseBlock.generated.h"

class UBlockState;
class UBlockDataContainer;

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
	
	//ダメージを受ける処理
	bool TakeDamage(FAttackData, float damage = 0)override;

private:
	//出現させるアイテム名
	UPROPERTY(EditAnywhere)
	FString DropItemName;

	//管理するステート
	UPROPERTY()
	UBlockState* CurrentState;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> ContainerClass;

	UPROPERTY()
	TObjectPtr<UBlockDataContainer> Container;
};
