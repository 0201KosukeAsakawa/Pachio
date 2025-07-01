#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/IDamageable.h"
#include "Enemy/EnemyCategory.h"
#include "EnemyCharacter.generated.h"

class UBoxComponent;
class UMoveComponent;
class UPhysicsCalculator;
class UEnemyStateComponent;

UCLASS()
class PACHIO_API AEnemyCharacter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// 初期化関数 (StateID, DropItemID, OptionalなMaterialID)
	void Init(const EEnemyCategory stateID,const EEnemyCategory materialID);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
	UStaticMeshComponent* meshComponent;

private:
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	EEnemyCategory LogicID;
};
