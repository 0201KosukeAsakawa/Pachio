#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
	virtual void Init();

	UFUNCTION()
	virtual void Overlap(AActor* hitActor);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	UPROPERTY(EditAnywhere)
	float MoveSpeed;
};
