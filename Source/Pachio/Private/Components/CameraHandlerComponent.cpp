#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


UCameraHandlerComponent::UCameraHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Cameraコンポーネントを作成（SpringArmは使用しない）
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

    // グリッドサイズを広げる（画面1枚分のサイズ）
    GridSize = FVector2D(2000.f, 2000.f);  // お好みで調整
    InterpSpeed = 3.0f;  // 補間スピード（好みに合わせて）
}

void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
    // CameraをRootComponentにアタッチ
    Camera->SetupAttachment(RootComponent);

    // プレイヤーの初期グリッドを算出
    FVector PlayerLocation = GetOwner()->GetActorLocation();
    CurrentGrid = FIntPoint(
        FMath::FloorToInt(PlayerLocation.X / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Y / GridSize.Y)
    );

    // 初期カメラ位置（画面中心 + 高さ引き）
    TargetCameraLocation = FVector(
        CurrentGrid.X * GridSize.X + GridSize.X / 2,
        CurrentGrid.Y * GridSize.Y + GridSize.Y / 2,
        800.0f  // 高さ：カメラを引いて見下ろす
    );
    Camera->SetWorldLocation(TargetCameraLocation);
}

void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateCameraPosition(DeltaTime);
}

void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
    if (!Camera || !GetOwner()) return;

    FVector PlayerLocation = GetOwner()->GetActorLocation();

    // 現在のグリッド（X-Y平面）を算出
    FIntPoint NewGrid(
        FMath::FloorToInt(PlayerLocation.X / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Y / GridSize.Y)
    );

    // グリッドが変化したら、目標位置を再設定
    if (NewGrid != CurrentGrid)
    {
        CurrentGrid = NewGrid;

        TargetCameraLocation = FVector(
            CurrentGrid.X * GridSize.X + GridSize.X / 2,
            CurrentGrid.Y * GridSize.Y + GridSize.Y / 2,
            800.0f  // Zは固定（高さ）
        );
    }

    // カメラを滑らかに補間移動
    FVector CurrentLocation = Camera->GetComponentLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
    Camera->SetWorldLocation(NewLocation);
}
