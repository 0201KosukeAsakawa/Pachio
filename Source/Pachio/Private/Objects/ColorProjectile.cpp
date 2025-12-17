// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorProjectile.h"
#include "Interface/ColorFilterInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AColorProjectile::AColorProjectile()
    : ProjectileColor(FLinearColor::White)
    , bHasHit(false)
{
    PrimaryActorTick.bCanEverTick = true;

    // 衝突コンポーネント（球体）
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &AColorProjectile::OnHit);
    RootComponent = CollisionComponent;

    // メッシュコンポーネント
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CollisionComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 球体メッシュをアセットから設定（要パス調整）
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMesh.Succeeded())
    {
        MeshComponent->SetStaticMesh(SphereMesh.Object);
        MeshComponent->SetRelativeScale3D(FVector(0.3f));
    }

    // 投射物移動コンポーネント
    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = CollisionComponent;
    MovementComponent->InitialSpeed = 1500.0f;
    MovementComponent->MaxSpeed = 1500.0f;
    MovementComponent->bRotationFollowsVelocity = true;
    MovementComponent->bShouldBounce = false;
    MovementComponent->ProjectileGravityScale = 0.5f; // 重力の影響（調整可能）

    // 初期ライフスパン（5秒後に自動削除）
    InitialLifeSpan = 5.0f;
}

void AColorProjectile::BeginPlay()
{
    Super::BeginPlay();

    // ダイナミックマテリアルを生成
    if (MeshComponent && MeshComponent->GetNumMaterials() > 0)
    {
        DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), ProjectileColor);
        }
    }
}

void AColorProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AColorProjectile::Launch(const FVector& Direction, float Speed, const FLinearColor& InColor)
{
    ProjectileColor = InColor;

    // 色をマテリアルに適用
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), ProjectileColor);
    }

    // 速度を設定
    MovementComponent->InitialSpeed = Speed;
    MovementComponent->MaxSpeed = Speed;
    MovementComponent->Velocity = Direction.GetSafeNormal() * Speed;
}

void AColorProjectile::SetProjectileColor(const FLinearColor& InColor)
{
    ProjectileColor = InColor;

    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), ProjectileColor);
    }
}

void AColorProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    // 既に衝突処理済みの場合はスキップ
    if (bHasHit)
        return;

    bHasHit = true;

    // ヒットしたアクターがインターフェースを実装しているか確認
    if (OtherActor && OtherActor != this)
    {
        // IColorReceiverインターフェースを持つコンポーネントを検索
        TArray<UActorComponent*> Components;
        OtherActor->GetComponents(Components);

        for (UActorComponent* Component : Components)
        {
            // インターフェースを実装しているか確認
            IColorReactiveInterface* ColorReceiver = Cast<IColorReactiveInterface>(Component);
            if (ColorReceiver)
            {
                // 色を渡す（インターフェースメソッド経由）
                 ColorReceiver->ApplyColorWithMatching(ProjectileColor);

                UE_LOG(LogTemp, Log, TEXT("ColorProjectile hit: %s, Color applied"),
                    *OtherActor->GetName());
                break;
            }
        }
    }

    // エフェクトやサウンドをここで再生可能

    // 投射物を破棄
    Destroy();
}