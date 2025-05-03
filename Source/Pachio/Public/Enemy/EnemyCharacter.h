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

private:
	UPROPERTY()
	 UBoxComponent* BoxComponent;

	UPROPERTY()
	 UStaticMeshComponent* MeshComponent;

private:	
	UPROPERTY()
	UEnemyStateComponent* Logic;

};
