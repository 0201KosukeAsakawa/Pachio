// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OutlineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibrary.h"
UOutlineComponent::UOutlineComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // Tickを有効化
}

void UOutlineComponent::BeginPlay()
{
    Super::BeginPlay();
    InitMesh();
}

void UOutlineComponent::InitMesh()
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        CachedMesh = Owner->FindComponentByClass<UMeshComponent>();
        if (CachedMesh)
        {
            CachedMesh->SetCustomDepthStencilValue(StencilValue);
            CachedMesh->SetRenderCustomDepth(false); // 初期は非表示
        }
    }
}

void UOutlineComponent::SetStencilValue(int32 Value)
{
    StencilValue = Value;
    if (CachedMesh)
    {
        CachedMesh->SetCustomDepthStencilValue(Value);
    }
}

void UOutlineComponent::EnableOutline(bool bEnable)
{
    if (CachedMesh)
    {
        CachedMesh->SetRenderCustomDepth(bEnable);
    }
}

void UOutlineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CachedMesh)
    {
        bool bBehindWall = IsBehindWall();
        EnableOutline(bBehindWall);
    }
}

bool UOutlineComponent::IsBehindWall()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return false;

    FVector CameraLoc;
    FRotator CameraRot;
    PC->GetPlayerViewPoint(CameraLoc, CameraRot);

    FVector TargetLoc = GetOwner()->GetActorLocation();

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, TargetLoc, ECC_Visibility, Params);

    // 壁などに当たったら裏にいると判断
    return bHit && Hit.GetActor() != GetOwner();
}