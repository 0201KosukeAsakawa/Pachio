// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MoveControllableObject.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Components/MoveComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AMoveControllableObject::AMoveControllableObject()
{
	PrimaryActorTick.bCanEverTick = true;

	// RootComponent �ɂ��� BoxComponent ��쐬
	FootTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FootTrigger"));
	RootComponent = FootTrigger;  // �� �����d�v�I

	// �Փːݒ�Ȃ�
	FootTrigger->SetGenerateOverlapEvents(true);
	FootTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FootTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FootTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Overlap�C�x���g��o�C���h
	FootTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMoveControllableObject::OnFootBeginOverlap);
	FootTrigger->OnComponentEndOverlap.AddDynamic(this, &AMoveControllableObject::OnFootEndOverlap);
}


// Called when the game starts or when spawned
void AMoveControllableObject::BeginPlay()
{
	Super::BeginPlay();
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (!MoveComp)
	{
		MoveComp = NewObject<UMoveComponent>(this);
		UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
		MoveComp->Init(this, PlayerLogic);
	}
	if (PlayerInputData)
	{
		PlayerInputData->Init(Controller);
	}
}

// Called every frame
void AMoveControllableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// �v���C���[���̓o�C���h����
void AMoveControllableObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// UPlayerInputComponent ����Ǝ��̓��̓o�C���f�B���O������Ăяo��
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->BindInput<AMoveControllableObject>(PlayerInputComponent);
	}
}

void AMoveControllableObject::Movement(const FInputActionValue& Value)
{
	FVector direction = MoveComp->Movement(0, this, Value);
	UE_LOG(LogTemp, Log, TEXT("Direction: X=%.3f, Y=%.3f, Z=%.3f"), direction.X, direction.Y, direction.Z);

	float MovementScale = 10.0f;
	FVector MovementDelta = direction * MovementScale;

	// ���g�̈ړ��iAddMovementInput �ł͂Ȃ��A�����Փ˂�l�������I�t�Z�b�g�j
	FHitResult SelfHit;
	AddActorWorldOffset(MovementDelta, true, &SelfHit);

	if (SelfHit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocked by: %s"), *SelfHit.GetActor()->GetName());
		MovementDelta = FVector::ZeroVector; // �u���b�N���ꂽ�̂Œ�~
	}
	if(AttachedActors.IsEmpty())
		return;
	// ��ɏ���Ă���I�u�W�F�N�g�̈ړ�
	for (AActor* ActorOnTop : AttachedActors)
	{
		if (ActorOnTop)
		{
			FHitResult Hit;
			ActorOnTop->AddActorWorldOffset(MovementDelta, true, &Hit);

			if (Hit.IsValidBlockingHit())
			{
				UE_LOG(LogTemp, Warning, TEXT("Attached actor %s blocked by: %s"), *ActorOnTop->GetName(), *Hit.GetActor()->GetName());
			}
		}
	}
}

void AMoveControllableObject::Action(const FInputActionValue& Value)
{
	AControllableObjectBase::Action(Value);
}



void AMoveControllableObject::OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ActorHasTag(TEXT("Carryable")) || !OtherActor->ActorHasTag("Moveable"))
		return;
	if (OtherActor && OtherActor != this)
	{
		// このオブジェクトが "Carryable" タグを持っているか確認


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
	if (OtherActor && AttachedActors.Contains(OtherActor))
	{
		AttachedActors.Remove(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("Removed actor from top: %s"), *OtherActor->GetName());
	}
}
