#include "Objects/BaseBlock.h"
#include "Components/BlockState.h"
#include "FunctionLibrary.h"
#include "Components/BoxComponent.h"
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

    // コンテナの初期化
    if (!Container)
    {
        Container = NewObject<UBlockDataContainer>(this, ContainerClass);
        if (Container)
        {
            CurrentState = Container->CreateState(GetWorld(), StateID);
        }
    }

    // ステートの初期化
    if (CurrentState)
    {
        CurrentState->OnEnter(this, GetWorld());
    }

    // Collision コンポーネントの取得
    Collision = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "Box");

    if (Collision)
    {
        UE_LOG(LogTemp, Warning, TEXT("Collision Component found!"));
        Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseBlock::BeginOverlap);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Collision Component not found!"));
    }
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


void ABaseBlock::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !OtherActor->ActorHasTag("Player"))
        return;

    if (!Container)
        return;

    UBlockState* nextState = Container->CreateState(GetWorld(), "Delpeted");

    if (!nextState)
        return;

    CurrentState = nextState;
    CurrentState->OnEnter(this, GetWorld());
}
