// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ControllableObjectBase.h"
#include "Player/InGameController.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Components/MoveComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AControllableObjectBase::AControllableObjectBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// RootComponent にする BoxComponent を作成
	FootTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FootTrigger"));
	RootComponent = FootTrigger;  // ← ここ重要！

	// 衝突設定など
	FootTrigger->SetGenerateOverlapEvents(true);
	FootTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FootTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FootTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Overlapイベントをバインド
	FootTrigger->OnComponentBeginOverlap.AddDynamic(this, &AControllableObjectBase::OnFootBeginOverlap);
	FootTrigger->OnComponentEndOverlap.AddDynamic(this, &AControllableObjectBase::OnFootEndOverlap);
}


// Called when the game starts or when spawned
void AControllableObjectBase::BeginPlay()
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
void AControllableObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// プレイヤー入力バインド処理
void AControllableObjectBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	// UPlayerInputComponent から独自の入力バインディング処理を呼び出す
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->BindInput<AControllableObjectBase>(PlayerInputComponent);
	}
}

void AControllableObjectBase::Movement(const FInputActionValue& Value)
{
	FVector direction = MoveComp->Movement(0, this, Value);
	UE_LOG(LogTemp, Log, TEXT("Direction: X=%.3f, Y=%.3f, Z=%.3f"), direction.X, direction.Y, direction.Z);

	float MovementScale = 10.0f;
	FVector MovementDelta = direction * MovementScale;

	// 自身の移動（AddMovementInput ではなく、物理衝突を考慮したオフセット）
	FHitResult SelfHit;
	AddActorWorldOffset(MovementDelta, true, &SelfHit);

	if (SelfHit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocked by: %s"), *SelfHit.GetActor()->GetName());
		MovementDelta = FVector::ZeroVector; // ブロックされたので停止
	}

	// 上に乗っているオブジェクトの移動
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



void AControllableObjectBase::Action(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) // 入力が有効な場合（ボタンが押された場合など）
	{
		// このPawnを操作しているコントローラーを取得
		AController* OwningController = GetController();
		if (OwningController)
		{
			// AInGameController にキャスト（もし AInGameController がこのPlayerCharacterをPossessしている場合）
			AInGameController* InGameController = Cast<AInGameController>(OwningController);
			if (InGameController)
			{
				// コントローラーのTogglePossession関数を呼び出す
				InGameController->ReturnToOriginalPlayer();
			}
		}
	}
}

void AControllableObjectBase::OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (!AttachedActors.Contains(OtherActor))
		{
			AttachedActors.Add(OtherActor);
			UE_LOG(LogTemp, Log, TEXT("Added actor on top: %s"), *OtherActor->GetName());
		}
	}
}

void AControllableObjectBase::OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && AttachedActors.Contains(OtherActor))
	{
		AttachedActors.Remove(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("Removed actor from top: %s"), *OtherActor->GetName());
	}
}
