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

    // ここでローカル座標を取得
    FVector WorldPos = Camera->GetComponentLocation();  // カメラのワールド座標
    LocalLocation = SpringArm->GetComponentTransform().InverseTransformPosition(WorldPos);  // 親（SpringArm）に対するローカル座標

    // ローカル座標をデバッグ表示（必要に応じて）
    UE_LOG(LogTemp, Log, TEXT("Camera Local Position: %s"), *LocalLocation.ToString());

    if (Camera)
    {
        PreviousCameraY = Camera->GetComponentLocation().Y;
    }
}



void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

   // UpdateCameraPosition(DeltaTime);
}

void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
    if (!Camera || !GetOwner())
        return;

    // カメラの現在のワールド位置
    FVector CurrentCameraPos = Camera->GetComponentLocation();

    // オーナーの位置（プレイヤーの位置）
    FVector OwnerPos = GetOwner()->GetActorLocation();

    // オーナー→カメラのベクトル
    FVector OwnerToCamera = CurrentCameraPos - OwnerPos;

    // 進行方向（プレイヤーが向いている方向）
    FRotator OwnerRotation = GetOwner()->GetActorRotation();
    FVector ForwardDirection = OwnerRotation.Vector(); // プレイヤーの進行方向

    // カメラの初期位置を設定（オーナーから少し離れた位置に設定）
    if (OwnerToCamera.Size() == 0.0f)
    {
        // カメラがオーナーの位置にぴったり重なっている場合、カメラを少し離した位置にセット
        FVector InitialCameraPos = OwnerPos - ForwardDirection * 200.f;  // プレイヤーの進行方向に少し離れた位置
        Camera->SetWorldLocation(InitialCameraPos);
        OwnerToCamera = Camera->GetComponentLocation() - OwnerPos;  // 再計算
    }

    // 進行方向にレイキャスト
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    // オーナーの位置から少し進行方向に向かってレイキャスト
    FVector RayStart = OwnerPos;
    FVector RayEnd = RayStart + ForwardDirection * 200.f;  // 進行方向に少し離れた場所

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayStart, RayEnd, ECC_Visibility, Params);

    if (bHit)
    {
        // 進行方向に障害物があった場合、カメラを少し手前に戻す
        FVector NewCameraPos = CurrentCameraPos - OwnerToCamera.GetSafeNormal() * 10.0f; // 少し手前に戻す
        Camera->SetWorldLocation(NewCameraPos);
    }
    else
    {
        //// 障害物がない場合、ローカル座標をワールド座標に変換してからカメラの位置を設定
        //FVector WorldLocation = SpringArm->GetComponentTransform().TransformPosition(LocalLocation);
        //Camera->SetWorldLocation(WorldLocation);
    }
}
