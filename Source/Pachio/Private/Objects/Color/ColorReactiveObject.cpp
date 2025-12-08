#include "Objects/Color/ColorReactiveObject.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Manager/ColorManager.h"
#include "Manager/LevelManager.h"


// コンストラクタ：Tick はデフォルトで無効（基本的にリアルタイム更新不要）
AColorReactiveObject::AColorReactiveObject()
    : bEnableBeatAnimation(true)
{
    PrimaryActorTick.bCanEverTick = false;

    // オブジェクト色管理コンポーネントを生成
    ObjectColorComponent = CreateDefaultSubobject<UObjectColorComponent>(TEXT("ObjectColorComponent"));
}

// BeginPlay（ゲーム開始時）に初期化処理を実行
void AColorReactiveObject::BeginPlay()
{
    Super::BeginPlay();

    if (ObjectColorComponent)
    {
        // コンポーネントの初期化
        ObjectColorComponent->Initialize();
    }

    // 派生クラス用の初期化
    Initialize();
}

// オブジェクト固有の初期化処理（派生クラスでオーバーライド可能）
void AColorReactiveObject::Initialize()
{
    // 派生クラスで実装
}

// =======================
// IColorReactiveInterface の実装
// =======================

void AColorReactiveObject::ApplyColorWithMatching(const FLinearColor& NewColor)
{
    if (!ObjectColorComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("ObjectColorComponent is null in %s"), *GetName());
        return;
    }

    ObjectColorComponent->SetColor(NewColor);
}

void AColorReactiveObject::ResetColor()
{
    if (!ObjectColorComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("ObjectColorComponent is null in %s"), *GetName());
        return;
    }

    ObjectColorComponent->ResetColor();
}

void AColorReactiveObject::SetSelected(bool bIsSelected)
{
    if (!ObjectColorComponent)
    {
        return;
    }

    ObjectColorComponent->SetSelected(bIsSelected);
}

bool AColorReactiveObject::HasColorChanged() const
{
    return ObjectColorComponent && ObjectColorComponent->HasColorChanged();
}

bool AColorReactiveObject::IsChangeable() const
{
    return ObjectColorComponent && ObjectColorComponent->IsChangeable();
}

bool AColorReactiveObject::IsColorMatched() const
{
    return ObjectColorComponent && ObjectColorComponent->IsColorMatched();
}

FName AColorReactiveObject::GetColorEventID() const
{
    if (!ObjectColorComponent)
    {
        return NAME_None;
    }

    return ObjectColorComponent->GetColorEventID();
}

// =======================
// 状態管理
// =======================

FLinearColor AColorReactiveObject::GetCurrentColor() const
{
    if (!ObjectColorComponent)
    {
        return FLinearColor::White;
    }

    return ObjectColorComponent->GetCurrentColor();
}

FLinearColor AColorReactiveObject::GetInitialColor() const
{
    if (!ObjectColorComponent)
    {
        return FLinearColor::White;
    }

    return ObjectColorComponent->GetInitialColor();
}

// =======================
// 色処理の委譲メソッド
// =======================

void AColorReactiveObject::ProcessColorMatching(const FLinearColor& NewColor)
{
    if (!ObjectColorComponent)
    {
        return;
    }

    ObjectColorComponent->ProcessColorMatching(NewColor);
}