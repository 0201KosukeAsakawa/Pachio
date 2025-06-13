// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveBeltConveyor.h"
#include "Components/ColorReactiveComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/BoxComponent.h"
#include "Manager/LevelManager.h"

AColorReactiveBeltConveyor::AColorReactiveBeltConveyor()
{
    // このアクターが Tick を呼び出すことを設定
    PrimaryActorTick.bCanEverTick = true;

    // Box Component を作成
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxComponent->SetupAttachment(RootComponent); // アクターのルートコンポーネントに設定

    //// Box のサイズを設定
    //BoxComponent->SetBoxExtent(FVector(200.0f, 200.0f, 50.0f));

    // オーバーラップイベントのバインド
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AColorReactiveBeltConveyor::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AColorReactiveBeltConveyor::OnOverlapEnd);
}

void AColorReactiveBeltConveyor::Init()
{
    AColorReactiveObject::Init();
    CurrentDirection = direction;
}

void AColorReactiveBeltConveyor::Tick(float)
{
    for (UPhysicsCalculator* target : hitObject)
    {
        if (target)
        {
            // target はポインタなので、デリファレンスして使います
            target->AddForce(CurrentDirection, 1.f, true);
        }
    }
}

void AColorReactiveBeltConveyor::ColorAction(const FLinearColor InColor)
{
    ApplyColorToMaterial(InColor); // 色をマテリアルに適用
    if (!ColorReactiveComponent)
        return;

    // 通常の色一致チェック
    bColorMuch = ColorReactiveComponent->CheckColorMatch(InColor);
    if (bColorMuch)
    {
        CurrentDirection = direction;
    }
    else
    {
        // InColorの反転色を取得
        FLinearColor ComplementaryColor = GetComplementaryColor(InColor);

        // 反転色で色一致チェック
        bColorMuch = ColorReactiveComponent->CheckColorMatch(ComplementaryColor);
        if (bColorMuch)
        {
            // 反転色で一致した場合の処理
            CurrentDirection = -direction;
        }
    }
}

FLinearColor AColorReactiveBeltConveyor::GetComplementaryColor(const FLinearColor& InColor)
{
    // RGBそれぞれを反転
    return FLinearColor(1.0f - InColor.R, 1.0f - InColor.G, 1.0f - InColor.B, InColor.A);
}

void AColorReactiveBeltConveyor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // まずOtherActorがnullptrかどうかをチェック
    if (OtherActor == nullptr)
        return;

    // 物理計算コンポーネント（UPhysicsCalculator）がアタッチされているか確認
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return; // UPhysicsCalculatorがアタッチされていない場合は何もしない

    // すでにhitObjectに同じオブジェクトが登録されているかを確認
    if (!hitObject.Contains(PhysicsCalculator))
    {
        // 同じオブジェクトが登録されていない場合、hitObjectに追加
        hitObject.Add(PhysicsCalculator);
    }
}


void AColorReactiveBeltConveyor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == nullptr)
        return;

    // UPhysicsCalculatorコンポーネントを取得
    UPhysicsCalculator* PhysicsCalculator = OtherActor->FindComponentByClass<UPhysicsCalculator>();
    if (PhysicsCalculator == nullptr)
        return; // UPhysicsCalculatorがアタッチされていない場合

    // hitObject から PhysicsCalculator を削除
    hitObject.Remove(PhysicsCalculator); // TArrayの場合
}