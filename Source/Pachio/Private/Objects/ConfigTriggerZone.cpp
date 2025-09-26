// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ConfigTriggerZone.h"
#include "Player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CameraHandlerComponent.h"

// Sets default values
AConfigTriggerZone::AConfigTriggerZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Box Collision �Z�b�g�A�b�v
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	// �q�b�g�C�x���g��o�C���h
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AConfigTriggerZone::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AConfigTriggerZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AConfigTriggerZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
		return;

	APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
	if (player == nullptr)
		return;
	UCameraHandlerComponent* cameraHandle = player->GetComponentByClass<UCameraHandlerComponent>();
	if (cameraHandle == nullptr)
		return;
	if (!cameraHandle->IsParameterMatch(GridSize, ZBaffer))
		cameraHandle->ApplyCameraSettings(GridSize, ZBaffer, CameraViewType);
}