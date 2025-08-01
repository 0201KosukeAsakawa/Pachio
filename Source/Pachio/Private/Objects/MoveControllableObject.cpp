// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MoveControllableObject.h"
#include "Components/MoveComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CameraHandlerComponent.h"
#include "Components/ColorConfigurator.h"
#include "Sound/SoundManager.h"
#include "Manager/LevelManager.h"


AMoveControllableObject::AMoveControllableObject()
{
	PrimaryActorTick.bCanEverTick = true;
	FootTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FootTrigger"));
	RootComponent = FootTrigger;

	FootTrigger->SetGenerateOverlapEvents(true);
	FootTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FootTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FootTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	FootTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMoveControllableObject::OnFootBeginOverlap);
	FootTrigger->OnComponentEndOverlap.AddDynamic(this, &AMoveControllableObject::OnFootEndOverlap);
}

void AMoveControllableObject::Init()
{
	AColorReactiveObject::Init();
	const TObjectPtr<USoundManager> SoundManager = Cast<USoundManager>(ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetObject());
	if (!SoundManager) return;

	SoundManager->OnBeatDetected.AddDynamic(this, &AMoveControllableObject::OnBeatDetected);
}

void AMoveControllableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving)
		return;

	FVector PreviousLocation = GetActorLocation();  // 移動前の位置を保持

	MoveElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(MoveElapsedTime / MoveDuration, 0.f, 1.f);

	FVector NewLocation = FMath::Lerp(MoveStartLocation, MoveTargetLocation, Alpha);
	SetActorLocation(NewLocation);

	FVector Offset = NewLocation - PreviousLocation;  // 今回の移動量を計算

	for (AActor* ActorOnTop : AttachedActors)
	{
		if (ActorOnTop)
		{
			ActorOnTop->AddActorWorldOffset(Offset);
		}
	}

	if (Alpha >= 1.f)
	{
		bIsMoving = false;
	}
}

void AMoveControllableObject::OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
		return;

	if (!ActorHasTag(TEXT("Carryable")) || !OtherActor->ActorHasTag("Moveable"))
		return;

	if (OtherActor && OtherActor != this)
	{
		if (!AttachedActors.Contains(OtherActor))
		{
			AttachedActors.Add(OtherActor);
			UE_LOG(LogTemp, Log, TEXT("Added actor on top: %s"), *OtherActor->GetName());
		}
	}
}

void AMoveControllableObject::OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
		return;

	if (OtherActor && AttachedActors.Contains(OtherActor))
	{
		AttachedActors.Remove(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("Removed actor from top: %s"), *OtherActor->GetName());
	}
}

void AMoveControllableObject::OnBeatDetected()
{
	if (PatrolPoints.Num() == 0 || bIsMoving)
		return;

	if (BeatCount > PlayCount)
	{
		++PlayCount;
		return;
	}
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = PatrolPoints[CurrentPatrolIndex];
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	if (Distance <= AcceptanceRadius)
	{
		CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
		TargetLocation = PatrolPoints[CurrentPatrolIndex];
	}

	MoveStartLocation = CurrentLocation;
	// 目標地点までの方向を取ってMoveStepSize分だけ進む目標位置を計算
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	MoveTargetLocation = CurrentLocation + Direction * MoveStepSize;

	bIsMoving = true;
	MoveElapsedTime = 0.0f;

	PlayCount = 0;
}
