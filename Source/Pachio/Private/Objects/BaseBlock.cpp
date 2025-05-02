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

    Init(StateID, DropItemID);

    // Collision �R���|�[�l���g�̎擾
    Collision = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "Box");

    if (IsValid(Collision))
    {
        UE_LOG(LogTemp, Warning, TEXT("Collision Component found!"));
        Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseBlock::BeginOverlap);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Collision Component not found!"));
    }
}

void ABaseBlock::Init(FString stateID, FString dorpItemID, FString materialID)
{
    StateID = stateID;
    DropItemID = dorpItemID;

    // �R���e�i�̏�����
    if (!Container)
    {
        Container = NewObject<UBlockDataContainer>(this, ContainerClass);

    }
    if (Container)
    {
        CurrentState = Container->CreateState(GetWorld(), StateID);
    }
    // �X�e�[�g�̏�����
    if (CurrentState)
    {
        if (materialID == "None")
            CurrentState->OnEnter(this, GetWorld(), Container);
        else
            CurrentState->OnEnter(this, GetWorld(), Container, materialID);
    }
}
// Called every frame
void ABaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

bool ABaseBlock::TakeDamage(FAttackData attackData, float damage)
{
	//if (CurrentState)
	//{
	//	CurrentState->OnHit(FVector(0, 0, 0), attackData);
	//}

	return true;
}

void ABaseBlock::ChangeState(UBlockState* nextState)
{
    if (CurrentState)
        CurrentState->OnExit(this);

    if (nextState)
        CurrentState = nextState;

    if (Container)
        CurrentState->OnEnter(this, GetWorld(), Container);
}


void ABaseBlock::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (CurrentState)
    {
        CurrentState->OnUpperAttack(OtherActor, FVector(0, 0, 0));
    }
}
