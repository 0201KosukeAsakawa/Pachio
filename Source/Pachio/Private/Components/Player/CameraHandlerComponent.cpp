#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// コンストラクタ: 初期化
UCameraHandlerComponent::UCameraHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // Tick有効

    // カメラコンポーネント生成
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

    // 補間スピード初期値
    InterpSpeed = 3.0f;
}

// 初期化: カメラの位置を設定
void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
    if (Camera == nullptr || RootComponent == nullptr)
        return;

    // 初期位置を設定
    SetCameraLocation(CameraViewType);
}

// Tick関数: 毎フレームカメラ位置を更新
void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCameraPosition(DeltaTime);
}

// カメラ追従処理
void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
    if (!Camera || !GetOwner())
        return;

    FVector PlayerLocation = GetOwner()->GetActorLocation();

    switch (CameraViewType)
    {
    case ECameraViewType::CharacterView:
    {
        // キャラクタービュー: プレイヤー前方＋少し上にカメラを配置
        FVector PlayerForward = GetOwner()->GetActorForwardVector();
        FVector AdjustedLocation = PlayerLocation + PlayerForward * 50.f;
        AdjustedLocation.Z += 50.f;

        FVector CameraBaseLocation(-Zbaffa, AdjustedLocation.Y, AdjustedLocation.Z);
        FVector PlayerOffset(0.f,
            AdjustedLocation.Y - CameraBaseLocation.Y,
            AdjustedLocation.Z - CameraBaseLocation.Z);

        // ゆるく追従
        TargetCameraLocation = CameraBaseLocation + PlayerOffset * 0.2f;
        break;
    }
    case ECameraViewType::GridView:
    {
        // グリッドビュー: プレイヤーの現在グリッドを計算（YZ平面）
        FIntPoint NewGrid(
            FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
            FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
        );

        if (NewGrid != CurrentGrid)
        {
            CurrentGrid = NewGrid;
        }

        FVector GridCenter(
            -Zbaffa,
            CurrentGrid.X * GridSize.X + GridSize.X / 2,
            CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
        );

        FVector PlayerOffset(
            0.f,
            PlayerLocation.Y - GridCenter.Y,
            PlayerLocation.Z - GridCenter.Z
        );

        // ゆるく追従
        TargetCameraLocation = GridCenter + PlayerOffset * 0.2f;
        break;
    }
    default:
        break;
    }

    // 現在位置から補間して滑らかに移動
    FVector CurrentLocation = Camera->GetComponentLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
    Camera->SetWorldLocation(NewLocation);
}

// カメラ位置を直接セット（初期化や切替時）
void UCameraHandlerComponent::SetCameraLocation(ECameraViewType type)
{
    FVector PlayerLocation = GetOwner()->GetActorLocation();

    // グリッドの初期位置計算
    CurrentGrid = FIntPoint(
        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    );

    Camera->SetWorldLocation(TargetCameraLocation);
}

// カメラ設定を適用（グリッドサイズとオフセット）
void UCameraHandlerComponent::ApplyCameraSettings(FVector2D newSize, float newBuffa)
{
    GridSize = newSize;
    Zbaffa = newBuffa;

    FVector PlayerLocation = GetOwner()->GetActorLocation();

    // 現在グリッドを更新
    CurrentGrid = FIntPoint(
        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    );

    // カメラ位置をグリッド中央＋プレイヤー偏差で計算
    TargetCameraLocation = FVector(
        -Zbaffa,
        CurrentGrid.X * GridSize.X + GridSize.X / 2,
        CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
    );

    Camera->SetWorldLocation(TargetCameraLocation);
}

// カメラ設定を適用（グリッドサイズ・オフセット・ビュータイプ）
void UCameraHandlerComponent::ApplyCameraSettings(FVector2D newSize, float newBuffa, ECameraViewType newViewType)
{
    CameraViewType = newViewType;
    GridSize = newSize;
    Zbaffa = newBuffa;

    SetCameraLocation(CameraViewType);
}

// 現在のカメラ設定と一致しているか確認
bool UCameraHandlerComponent::IsParameterMatch(FVector2D newSize, float newBuffa , ECameraViewType newType)
{
    return !(GridSize != newSize || Zbaffa != newBuffa || CameraViewType != newType);
}
