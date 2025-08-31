// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MoveControllableObject.h"
#include "Components/MoveComponent.h"
#include "Components/Player/PlayerInputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CameraHandlerComponent.h"
#include "Components/Color/ColorConfigurator.h"
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

	// デフォルト設定
	MoveDuration = 0.5f;
	AcceptanceRadius = 10.0f;
	MoveStepSize = 100.0f;
}

void AMoveControllableObject::Init()
{
	AColorReactiveObject::Init();

	const TObjectPtr<USoundManager> SoundManager =
		Cast<USoundManager>(ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetObject());
	if (!SoundManager) return;

	SoundManager->OnBeatDetected.AddDynamic(this, &AMoveControllableObject::OnBeatDetected);
}

void AMoveControllableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bPlayBeat)
	{
		if (!bIsMoving)
			return;
	}

	if (bCanMove)
	{
		Movement(DeltaTime);
		Check();
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

	if (OtherActor && OtherActor != this && !AttachedActors.Contains(OtherActor))
	{
		AttachedActors.Add(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("Added actor on top: %s"), *OtherActor->GetName());
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

	// BeatCount / PlayCount の条件
	if (BeatCount > PlayCount)
	{
		++PlayCount;
		return;
	}

	Check();

	bIsMoving = true;
	MoveElapsedTime = 0.0f;
	PlayCount = 0;
}

void AMoveControllableObject::ColorAction(FLinearColor color, FEffectMatchResult result)
{
	AColorReactiveObject::ColorAction(color,result);
	
	ColorConfigurator->IsColorMatch(color, FLinearColor(1, 0.7, 0.8, 1)) ? bCanMove = false : bCanMove = true;
}

void AMoveControllableObject::Movement(float DeltaTime)
{
	FVector PreviousLocation = GetActorLocation();

	MoveElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(MoveElapsedTime / MoveDuration, 0.f, 1.f);

	FVector NewLocation = FMath::Lerp(MoveStartLocation, MoveTargetLocation, Alpha);
	SetActorLocation(NewLocation);

	// 上に乗っているアクターを一緒に動かす
	FVector Offset = NewLocation - PreviousLocation;
	TArray<AActor*> target = AttachedActors;
	for (AActor* ActorOnTop : target)
	{
		if (ActorOnTop)
		{
			ActorOnTop->AddActorWorldOffset(Offset, true);
		}
	}
	TArray<AActor*> children = Child;
	for (AActor* ChildActor : children)
	{
		if (ChildActor)
		{
			ChildActor->AddActorWorldOffset(Offset);
		}
	}

	if (Alpha >= 1.f)
	{
		bIsMoving = false;
	}
}

void AMoveControllableObject::Check()
{
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = PatrolPoints[CurrentPatrolIndex];
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	// 目的地到達判定
	if (Distance <= AcceptanceRadius)
	{
		CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
		TargetLocation = PatrolPoints[CurrentPatrolIndex];
	}

	MoveStartLocation = CurrentLocation;

	if (bUseStepMove)
	{
		// MoveStepSize分だけ進む
		FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
		if (!Direction.IsNearlyZero())
		{
			MoveTargetLocation = CurrentLocation + Direction * MoveStepSize;
		}
		else
		{
			MoveTargetLocation = TargetLocation;
		}
	}
	else
	{
		// 目的地まで直接移動
		MoveTargetLocation = TargetLocation;
	}
}
