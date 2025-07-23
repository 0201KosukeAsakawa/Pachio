// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/RotateControllableObject.h"
#include "Components/PlayerInputComponent.h"

ARotateControllableObject::ARotateControllableObject()
{
}

// Called when the game starts or when spawned
void ARotateControllableObject::BeginPlay()
{
    Super::BeginPlay();
    UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
    
    if (PlayerInputData)
    {
        PlayerInputData->Init(Controller);
    }
}

void ARotateControllableObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // UPlayerInputComponent ����Ǝ��̓��̓o�C���f�B���O������Ăяo��
    UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
    if (PlayerInputData)
    {
        PlayerInputData->BindInput<ARotateControllableObject>(PlayerInputComponent);
    }
}

void ARotateControllableObject::Movement(const FInputActionValue& Value)
{
    FVector2D InputVec = Value.Get<FVector2D>();  // 2D入力を取得

    float InputX = InputVec.Y;  // 左右入力

    if (FMath::IsNearlyZero(InputX, 0.01f))
        return;

    const float RotationSpeed = 90.f; // 1秒あたり90度回転
    float DeltaAngle = InputX * RotationSpeed * GetWorld()->GetDeltaSeconds();

    FRotator CurrentRotation = GetActorRotation();
    FRotator DeltaRotation = direction* DeltaAngle;
    FRotator NewRotation = CurrentRotation + DeltaRotation;

    UpdateBsRelativeToA(DeltaRotation);

    SetActorRotation(NewRotation);
}

void ARotateControllableObject::Action(const FInputActionValue& Value)
{
    AControllableObjectBase::Action(Value);
}

void ARotateControllableObject::UpdateBsRelativeToA(const FRotator& DeltaRotation)
{
    if (TargetArray.Num() == 0)
    {
        return;
    }

    FVector Center = GetActorLocation();

    for (const FTargetData& TargetData : TargetArray)
    {
        AActor* BActor = TargetData.targetActor;
        if (!BActor)
            continue;

        // ���x�𔽉f������]�N�H�[�^�j�I��
        FRotator ScaledRotation = DeltaRotation * TargetData.rotateSpeed;
        FQuat DeltaQuat = ScaledRotation.Quaternion();

        // �ʒu���]
        FVector Relative = BActor->GetActorLocation() - Center;
        FVector Rotated = DeltaQuat.RotateVector(Relative);
        BActor->SetActorLocation(Center + Rotated);

        // ��]�������i�p�x�ύX�j
        if (bShouldRotate)
        {
            FQuat CurrentQuat = BActor->GetActorQuat();
            FQuat NewQuat = DeltaQuat * CurrentQuat;
            BActor->SetActorRotation(NewQuat);
        }
    }
}