#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


UCameraHandlerComponent::UCameraHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // SpringArm と Camera コンポーネントを作成
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

    // SpringArmのカメラ衝突テストを有効化
    SpringArm->bDoCollisionTest = true;  // 衝突判定ON
    SpringArm->ProbeChannel = ECC_Camera; // 衝突検出に使うチャンネル（プロジェクトに合わせて調整）
    SpringArm->TargetArmLength = 300.f;  // カメラとプレイヤーの距離（任意調整）
}

void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
    // SpringArmをRootComponentにアタッチ
    SpringArm->SetupAttachment(GetOwner()->GetRootComponent());

    // CameraをSpringArmにアタッチ
    Camera->SetupAttachment(SpringArm);

    if (Camera)
    {
        PreviousCameraY = Camera->GetComponentLocation().Y;
    }
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

    // カメラの現在のワールド位置
    FVector CurrentCameraPos = Camera->GetComponentLocation();

    // オーナーの位置（カメラ基準点）
    FVector OwnerPos = GetOwner()->GetActorLocation();

    // オーナー→カメラのベクトル
    FVector OwnerToCamera = CurrentCameraPos - OwnerPos;

    // レイキャスト開始点はオーナー位置、終点はカメラの現在位置
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, OwnerPos, CurrentCameraPos, ECC_Visibility, Params);

    if (bHit)
    {
        // 障害物があったら、少し手前にカメラを戻す
        FVector NewCameraPos = HitResult.Location - OwnerToCamera.GetSafeNormal() * 10.0f;
        Camera->SetWorldLocation(HitResult.Location);
    }
    else
    {
        // 障害物がなければカメラの位置はそのまま維持
        Camera->SetWorldLocation(CurrentCameraPos);
    }
}
