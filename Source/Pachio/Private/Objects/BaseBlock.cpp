#include "Objects/BaseBlock.h"
#include "Components/BlockState.h"
#include "DataContainer/BlockDataContainer.h"

// Sets default values
ABaseBlock::ABaseBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseBlock::BeginPlay()
{
	Super::BeginPlay();
	if (Container)
		CurrentState = Container->CreateState(GetWorld(), "Idle");
}

// Called every frame
void ABaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseBlock::TakeDamage(FAttackData attackData, float damage)
{
	if (CurrentState)
	{
		CurrentState->OnHit(FVector(0, 0, 0), attackData);
	}

	return true;
}


