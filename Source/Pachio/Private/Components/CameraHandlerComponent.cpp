#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UCameraHandlerComponent::UCameraHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

    InterpSpeed = 3.0f;
}

void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
    Camera->SetupAttachment(RootComponent);

    FVector PlayerLocation = GetOwner()->GetActorLocation();

    // Y: 横方向 / Z: 縦方向
    CurrentGrid = FIntPoint(
        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    );

    TargetCameraLocation = FVector(
        -Zbaffa,  // ← X方向に配置（プレイヤーの右側）
        CurrentGrid.X * GridSize.X + GridSize.X / 2,
        CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
    );

    Camera->SetWorldLocation(TargetCameraLocation);

    // プレイヤーの方向に向ける（左を向く = -X）
    Camera->SetWorldRotation(FRotator(0.f, -90.f, 45.f));

    Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

}



void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateCameraPosition(DeltaTime);
}

void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
    if (!Camera || !GetOwner())
        return;

    FVector PlayerLocation = GetOwner()->GetActorLocation();

    FIntPoint NewGrid(
        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    );

    if (NewGrid != CurrentGrid)
    {
        CurrentGrid = NewGrid;

        TargetCameraLocation = FVector(
            -Zbaffa,  // ← プレイヤーの右側に固定配置（X方向）
            CurrentGrid.X * GridSize.X + GridSize.X / 2,
            CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
        );
    }

    FVector CurrentLocation = Camera->GetComponentLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
    Camera->SetWorldLocation(NewLocation);
}

void UCameraHandlerComponent::Set(FVector2D newSize, float newBuffa)
{
    GridSize = newSize;
    Zbaffa = newBuffa;
    FVector PlayerLocation = GetOwner()->GetActorLocation();
    // Y: 横方向 / Z: 縦方向
    CurrentGrid = FIntPoint(
        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    );

    TargetCameraLocation = FVector(
        -Zbaffa,  // ← X方向に配置（プレイヤーの右側）
        CurrentGrid.X * GridSize.X + GridSize.X / 2,
        CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
    );

    Camera->SetWorldLocation(TargetCameraLocation);
}