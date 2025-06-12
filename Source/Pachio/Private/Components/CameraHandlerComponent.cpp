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

    CameraLocation = Camera->GetComponentLocation();
}



void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
}

void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{

}
