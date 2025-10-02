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
  //  SetComponentTickEnabled(false);
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

    switch (CameraViewType)
    {
    case ECameraViewType::CharacterView:
    {
        // プレイヤーの前方ベクトルを取得
        FVector PlayerForward = GetOwner()->GetActorForwardVector();

        // プレイヤーの前方+50、高さ+50した位置を基準にする
        FVector AdjustedLocation = PlayerLocation + PlayerForward * 50.f;
        AdjustedLocation.Z += 50.f;

        // カメラのX座標は固定（元コードの符号に合わせて-Zbaffa）
        FVector CameraBaseLocation(-Zbaffa, AdjustedLocation.Y, AdjustedLocation.Z);

        FVector PlayerOffset(0.f,
            AdjustedLocation.Y - CameraBaseLocation.Y,
            AdjustedLocation.Z - CameraBaseLocation.Z
        );

        // ゆるく追尾
        TargetCameraLocation = CameraBaseLocation + PlayerOffset * 0.2f;
        break;
    }
    case ECameraViewType::GridView:
        FIntPoint NewGrid;
        // グリッド計算（YZ平面）
        NewGrid = FIntPoint(
            FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
            FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
        );

        if (NewGrid != CurrentGrid)
        {
            CurrentGrid = NewGrid;
        }

        // グリッド中央 + プレイヤー位置の偏差 * 追尾割合
        {
            FVector GridCenter(
                -Zbaffa,
                CurrentGrid.X * GridSize.X + GridSize.X / 2,
                CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
            );

            FVector PlayerOffset(0.f,
                PlayerLocation.Y - GridCenter.Y,
                PlayerLocation.Z - GridCenter.Z
            );

            // 0.2f くらいにすると「ゆるく追尾」
            TargetCameraLocation = GridCenter + PlayerOffset * 0.2f;
        }
        break;
    default:
        break;
    }

    // 補間移動
    FVector CurrentLocation = Camera->GetComponentLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
    Camera->SetWorldLocation(NewLocation);
}


//void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
//{
//    if (!Camera || !GetOwner())
//        return;
//
//    FVector PlayerLocation = GetOwner()->GetActorLocation();
//    FIntPoint NewGrid;
//
//    switch (CameraViewType)
//    {
//    case ECameraViewType::SideView:
//        // グリッド計算（YZ平面）
//        NewGrid = FIntPoint(
//            FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
//            FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
//        );
//
//        if (NewGrid != CurrentGrid)
//        {
//            CurrentGrid = NewGrid;
//        }
//
//        // グリッド中央 + プレイヤー位置の偏差 * 追尾割合
//        {
//            FVector GridCenter(
//                -Zbaffa,
//                CurrentGrid.X * GridSize.X + GridSize.X / 2,
//                CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
//            );
//
//            FVector PlayerOffset(0.f,
//                PlayerLocation.Y - GridCenter.Y,
//                PlayerLocation.Z - GridCenter.Z
//            );
//
//            // 0.2f くらいにすると「ゆるく追尾」
//            TargetCameraLocation = GridCenter + PlayerOffset * 0.2f;
//        }
//        break;
//
//    case ECameraViewType::TopView:
//        NewGrid = FIntPoint(
//            FMath::FloorToInt(PlayerLocation.X / GridSize.X),
//            FMath::FloorToInt(PlayerLocation.Y / GridSize.Y)
//        );
//
//        if (NewGrid != CurrentGrid)
//        {
//            CurrentGrid = NewGrid;
//        }
//
//        {
//            FVector GridCenter(
//                CurrentGrid.X * GridSize.X + GridSize.X / 2,
//                CurrentGrid.Y * GridSize.Y + GridSize.Y / 2,
//                Zbaffa
//            );
//
//            FVector PlayerOffset(
//                PlayerLocation.X - GridCenter.X,
//                PlayerLocation.Y - GridCenter.Y,
//                0.f
//            );
//
//            TargetCameraLocation = GridCenter + PlayerOffset * 0.2f;
//        }
//        break;
//    default:
//        break;
//    }
//
//    // 補間移動
//    FVector CurrentLocation = Camera->GetComponentLocation();
//    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
//    Camera->SetWorldLocation(NewLocation);
//}

void UCameraHandlerComponent::SetCameraRotation(ECameraViewType type)
{
    //switch (type)
    //{
    //case ECameraViewType::SideView:
    //    // プレイヤーの方向に向ける（左を向く = -X）
    //    Camera->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
    //    break;
    //case ECameraViewType::TopView:
    //    // プレイヤーの方向に向ける（左を向く = -X）
    //    Camera->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
    //    break;
    //}
}

void UCameraHandlerComponent::SetCameraLocation(ECameraViewType type)
{
    FVector PlayerLocation = GetOwner()->GetActorLocation();
    CurrentGrid = FIntPoint(
        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    );
    //switch (type)
    //{
    //case ECameraViewType::SideView:
    //    // Y: 横方向 / Z: 縦方向
    //    CurrentGrid = FIntPoint(
    //        FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
    //        FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
    //    );

    //    TargetCameraLocation = FVector(
    //        -Zbaffa,  // ← X方向に配置（プレイヤーの右側）
    //        CurrentGrid.X * GridSize.X + GridSize.X / 2,
    //        CurrentGrid.Y * GridSize.Y + GridSize.Y / 2
    //    );
    //    break;
    //case ECameraViewType::TopView:
    //    CurrentGrid = FIntPoint(
    //        FMath::FloorToInt(PlayerLocation.X / GridSize.X), // X軸
    //        FMath::FloorToInt(PlayerLocation.Y / GridSize.Y)  // Y軸
    //    );

    //    TargetCameraLocation = FVector(
    //        CurrentGrid.X * GridSize.X + GridSize.X / 2,
    //        CurrentGrid.Y * GridSize.Y + GridSize.Y / 2,
    //        Zbaffa  // 高さ（上空から）
    //    );
    //    break;
    //default:
    //    break;
    //}

    Camera->SetWorldLocation(TargetCameraLocation);
}

void UCameraHandlerComponent::ApplyCameraSettings(FVector2D newSize, float newBuffa)
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

void UCameraHandlerComponent::ApplyCameraSettings(FVector2D newSize, float newBuffa, ECameraViewType newViewType)
{
    CameraViewType = newViewType;
    GridSize = newSize;
    Zbaffa = newBuffa;
    SetCameraLocation(CameraViewType);
    SetCameraRotation(CameraViewType);
}

bool UCameraHandlerComponent::IsParameterMatch(FVector2D newSize, float newBuffa)
{
    //if(GridSize != newSize|| Zbaffa != newBuffa)
    //return false;

    //return true;

    return !(GridSize != newSize || Zbaffa != newBuffa);
}
