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
    if (Camera == nullptr || RootComponent == nullptr)
        return;

    SetCameraLocation(CameraViewType);
    SetCameraRotation(CameraViewType);
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
    FIntPoint NewGrid;

    switch (CameraViewType)
    {
    case ECameraViewType::SideView:
        // YZ平面
        NewGrid = FIntPoint(
            FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
            FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
        );

        if (NewGrid != CurrentGrid)
        {
            CurrentGrid = NewGrid;

            TargetCameraLocation = FVector(
                -Zbaffa,
                CurrentGrid.X * GridSize.X + GridSize.X / 2,
                CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
            );
        }
        break;

    case ECameraViewType::TopView:
        // XY平面
        NewGrid = FIntPoint(
            FMath::FloorToInt(PlayerLocation.X / GridSize.X),
            FMath::FloorToInt(PlayerLocation.Y / GridSize.Y)
        );

        if (NewGrid != CurrentGrid)
        {
            CurrentGrid = NewGrid;

            TargetCameraLocation = FVector(
                CurrentGrid.X * GridSize.X + GridSize.X / 2,
                CurrentGrid.Y * GridSize.Y + GridSize.Y / 2,
                Zbaffa
            );
        }
        break;

    default:
        break;
    }

    FVector CurrentLocation = Camera->GetComponentLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
    Camera->SetWorldLocation(NewLocation);
}

void UCameraHandlerComponent::SetCameraRotation(ECameraViewType type)
{
    switch (type)
    {
    case ECameraViewType::SideView:
        // プレイヤーの方向に向ける（左を向く = -X）
        Camera->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
        break;
    case ECameraViewType::TopView:
        // プレイヤーの方向に向ける（左を向く = -X）
        Camera->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
        break;
    }
}

void UCameraHandlerComponent::SetCameraLocation(ECameraViewType type)
{
    FVector PlayerLocation = GetOwner()->GetActorLocation();
    switch (type)
    {
    case ECameraViewType::SideView:
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
        break;
    case ECameraViewType::TopView:
        CurrentGrid = FIntPoint(
            FMath::FloorToInt(PlayerLocation.X / GridSize.X), // X軸
            FMath::FloorToInt(PlayerLocation.Y / GridSize.Y)  // Y軸
        );

        TargetCameraLocation = FVector(
            CurrentGrid.X * GridSize.X + GridSize.X / 2,
            CurrentGrid.Y * GridSize.Y + GridSize.Y / 2,
            Zbaffa  // 高さ（上空から）
        );
        break;
    default:
        break;
    }

    Camera->SetWorldLocation(TargetCameraLocation);
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