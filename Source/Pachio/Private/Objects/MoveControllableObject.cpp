// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MoveControllableObject.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Components/ColorConfigurator.h"
#include "Components/MoveComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CameraHandlerComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "Sound/SoundManager.h"
#include "Manager/ColorManager.h"
#include "Manager/LevelManager.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AMoveControllableObject::AMoveControllableObject()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraHandlerComponent = CreateDefaultSubobject<UCameraHandlerComponent>(TEXT("CameraHandlerComponent"));
	ColorConfigurator = CreateDefaultSubobject<UColorConfigurator>(TEXT("ColorConfigurator"));

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
	const TObjectPtr<USoundManager> SoundManager = Cast<USoundManager>(ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetObject());
	if (!SoundManager) return;

	SoundManager->OnBeatDetected.AddDynamic(this, &AMoveControllableObject::OnBeatDetected);

	if(ColorConfigurator)
	ColorConfigurator->Init();
	if (CameraHandlerComponent)
		CameraHandlerComponent->Init(RootComponent);
}


void AMoveControllableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving) return;

	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.f, 1.f);
	float EasedAlpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f); // Easing optional

	// 移動量を算出
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, EasedAlpha);
	FVector MovementDelta = NewLocation - CurrentLocation;

	SetActorLocation(NewLocation);

	// 上のアクターを一緒に動かす
	if (!AttachedActors.IsEmpty())
	{
		for (AActor* ActorOnTop : AttachedActors)
		{
			if (ActorOnTop)
			{
				FHitResult Hit;
				ActorOnTop->AddActorWorldOffset(MovementDelta, true, &Hit);

				if (Hit.IsValidBlockingHit())
				{
					UE_LOG(LogTemp, Warning, TEXT("Attached actor %s blocked by: %s"),
						*ActorOnTop->GetName(), *Hit.GetActor()->GetName());
				}
			}
		}
	}

	// 終了判定
	if (Alpha >= 1.f)
	{
		bIsMoving = false;
	}
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
	FVector2D InputVec = Value.Get<FVector2D>();
	if (InputVec.IsNearlyZero())
	{
		CurrentInputDirection = FVector::ZeroVector;
		return;
	}

	// 入力を2Dベクトルから3Dへ変換（例：X→軸方向のスケールに使う）
	// 今回は入力のY成分を軸方向のスケールとして使う例
	// もしスティックXでプラス・マイナスをつけたい場合はInputVec.Xを使うなど調整して下さい。

	// ここでは単純にInputVec.Yを軸方向に乗算している例です
	FVector NormalizedAxis = MovementAxis.GetSafeNormal();
	CurrentInputDirection = NormalizedAxis * InputVec.Y;

	// これで軸方向の正負方向に移動できます
}


void AMoveControllableObject::Action(const FInputActionValue& Value)
{
	AControllableObjectBase::Action(Value);
}



void AMoveControllableObject::OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// OtherCompがnullptrでなく、タグ"Hoge"を持っていたら処理しない
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
	// OtherCompがnullptrでなく、タグ"Hoge"を持っていたら処理しない
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
		return;

	if (OtherActor && AttachedActors.Contains(OtherActor))
	{
		AttachedActors.Remove(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("Removed actor from top: %s"), *OtherActor->GetName());
	}
}

void AMoveControllableObject::ColorAction(FLinearColor InColor)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->ColorAction(InColor);
	ApplyEffectFromColor(InColor);
}

void AMoveControllableObject::SetColor(FLinearColor color)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->SetColor(color);
}

void AMoveControllableObject::ResetColor()
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->ResetColor();
}

bool AMoveControllableObject::IsColorChange() const
{
	if (ColorConfigurator == nullptr)
		return false;

	return ColorConfigurator->IsColorChange();
	
}

void AMoveControllableObject::SetSelectMode(bool b)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->SetSelectMode(b);
}

void AMoveControllableObject::ChangeLock(bool b)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->ChangeLock(b);
}



void AMoveControllableObject::ExecuteMovement(const FVector& Direction)
{
	FVector DirectionVector = Direction;
	if (MoveComp)
	{
		DirectionVector = MoveComp->Movement(0, this, FInputActionValue(Direction));
	}

	FVector MovementDelta = DirectionVector * MovementScale;

	FHitResult SelfHit;
	AddActorWorldOffset(MovementDelta, true, &SelfHit);

	if (SelfHit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocked by: %s"), *SelfHit.GetActor()->GetName());
		MovementDelta = FVector::ZeroVector;
	}

	if (AttachedActors.IsEmpty())
		return;

	for (AActor* ActorOnTop : AttachedActors)
	{
		if (!ActorOnTop) continue;

		FHitResult Hit;
		ActorOnTop->AddActorWorldOffset(MovementDelta, true, &Hit);

		if (Hit.IsValidBlockingHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("Attached actor %s blocked by: %s"),
				*ActorOnTop->GetName(), *Hit.GetActor()->GetName());
		}
	}
}

void AMoveControllableObject::OnBeatDetected()
{
	if (CurrentInputDirection.IsNearlyZero()) return;

	FVector Start = GetActorLocation();
	FVector End = Start + CurrentInputDirection * MovementScale;

	if (CanMoveToTarget(Start, End))
	{
		StartLocation = Start;
		TargetLocation = End;
		ElapsedTime = 0.f;
		bIsMoving = true;
	}
}

void AMoveControllableObject::ApplyEffectFromColor(const FLinearColor& Color)
{
    // 色から最も近いバフ効果と強度を取得
    FEffectMatchResult Match = ALevelManager::GetInstance(GetWorld())
        ->GetColorManager()
        ->GetClosestEffectByHue(Color);

    // EBuffEffect と EAxisType の対応表
    static const TMap<EBuffEffect, EAxisType> EffectToAxisMap = {
        { EBuffEffect::Red,   EAxisType::X },
        { EBuffEffect::Green, EAxisType::Y },
        { EBuffEffect::Blue,  EAxisType::Z }
    };

    if (const EAxisType* Axis = EffectToAxisMap.Find(Match.ClosestEffect))
    {
        if (AllowedAxes.Contains(*Axis))
        {
            switch (*Axis)
            {
            case EAxisType::X:
                MovementAxis = FVector(1, 0, 0);
                break;
            case EAxisType::Y:
                MovementAxis = FVector(0, 1, 0);
                break;
            case EAxisType::Z:
                MovementAxis = FVector(0, 0, 1);
                break;
            default:
                break;
            }
        }
    }
}



bool AMoveControllableObject::CanMoveToTarget(const FVector& Start, const FVector& End) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 自分自身は無視
	for (AActor* Actor : AttachedActors)
	{
		QueryParams.AddIgnoredActor(Actor); // 上のアクターも無視
	}

	FVector BoxExtent = GetCollisionBoxExtent();

	FHitResult Hit;
	FQuat Rotation = GetActorQuat(); // または GetRootComponent()->GetComponentQuat();

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		Rotation,
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		QueryParams
	);
	DrawDebugBox(
		GetWorld(),
		End,            // Boxの中心（例：終了地点）
		BoxExtent,
		Rotation,
		FColor::Blue,
		false,
		5.0f,
		0,
		2.0f
	);
	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocked by %s"), *Hit.GetActor()->GetName());
	}
	return !bHit;
}

FVector AMoveControllableObject::GetCollisionBoxExtent() const
{
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		// BoundsのBoxExtentを取得（ワールドスケール込みの半径的なサイズ）
		return PrimComp->Bounds.BoxExtent;
	}
	// デフォルト値
	return FVector(50.f, 50.f, 50.f);
}