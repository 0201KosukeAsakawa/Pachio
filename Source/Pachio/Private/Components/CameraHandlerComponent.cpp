// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
UCameraHandlerComponent::UCameraHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// SpringArmの作成とルートへのアタッチ
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Cameraの作成とSpringArmへのアタッチ
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}


// Called when the game starts
void UCameraHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{

	SpringArm->SetupAttachment(RootComponent);


	Camera->SetupAttachment(SpringArm);

	// SpringArmの初期設定
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->SocketOffset = FVector(0.0f, 100.0f, 50.0f);
	SpringArm->bUsePawnControlRotation = false; // プレイヤー回転と連動しない

	// SpringArm の回転継承設定
	if (SpringArm)
	{
		SpringArm->bInheritYaw = true;    // Yaw（左右）は継承する
		SpringArm->bInheritPitch = false; // Pitch（上下）は継承しない
		SpringArm->bInheritRoll = false;  // Roll（傾き）は継承しない
	}

	// カメラのY座標最大値を初期化（初期カメラ位置）
	if (Camera)
	{
		//カメラの初期位置を少し右にずれるようにして設定
		MaxCameraY = Camera->GetComponentLocation().Y + 1000;
		CameraXZ = Camera->GetComponentLocation();

		// カメラのY座標を固定（右スクロール固定）
		FVector CameraLocation = Camera->GetComponentLocation();

		CameraLocation = FVector(CameraXZ.X, MaxCameraY, CameraXZ.Z);
		Camera->SetWorldLocation(CameraLocation);
	}
}


// Called every frame
void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

