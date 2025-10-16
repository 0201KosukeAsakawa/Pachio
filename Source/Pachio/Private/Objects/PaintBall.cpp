// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PaintBall.h"
#include "Interface/ColorFilterInterface.h"
#include "Components/SphereComponent.h"

APaintBall::APaintBall()
{
    PrimaryActorTick.bCanEverTick = true;

    // 当たり判定用
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetGenerateOverlapEvents(true);
    //CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APaintBall::OnOverlap);
}

void APaintBall::BeginPlay()
{
    Super::BeginPlay();
}

void APaintBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 移動処理などはここに追加
}

void APaintBall::OnOverlap(AActor* OtherActor)
{
    if (!OtherActor || OtherActor == this) return;

    IColorReactiveInterface* Interface = Cast<IColorReactiveInterface>(OtherActor);
    if (Interface)
    {
        Interface->ApplyColorWithMatching(PaintColor);
        Destroy(); // 衝突したら消す場合
    }
}


