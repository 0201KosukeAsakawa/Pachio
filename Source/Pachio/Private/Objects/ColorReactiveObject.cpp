#include "Objects/ColorReactiveObject.h"
#include "Components/ColorConfigurator.h"

// コンストラクタ：Tick はデフォルトで無効（基本的にリアルタイム更新不要）
AColorReactiveObject::AColorReactiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
	ColorConfigurator = CreateDefaultSubobject<UColorConfigurator>(TEXT("ColorConfigurator"));
}

// BeginPlay（ゲーム開始時）に初期化処理を実行
void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	ColorConfigurator->Init();
	Init();
}

// 色反応オブジェクトの初期化処理
void AColorReactiveObject::Init()
{
	
}

// 色反応ロジックの初期化（UColorReactiveComponentの生成）
void AColorReactiveObject::InitializeColorLogic()
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->InitializeColorLogic();
}

// レベル上のカラーマネージャーに自身を登録
void AColorReactiveObject::RegisterToColorManager()
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->RegisterToColorManager();
}

// マテリアルとカスタムデプス設定
void AColorReactiveObject::SetupMaterial()
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->SetupMaterial();
}

void AColorReactiveObject::PlayBeatAnimation()
{
	if (ColorConfigurator == nullptr || !bPlayBeat)
		return;

	ColorConfigurator->PlayBeatAnimation();
}

// 色アクション実行時の処理（デフォルト実装）
void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->ColorAction(NewColor);
}

void AColorReactiveObject::SetColor(FLinearColor newColor)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->SetColor(newColor);
}

void AColorReactiveObject::ResetColor()
{
	if (ColorConfigurator == nullptr)
		return;

	//SetColor(StartColor);
}

void AColorReactiveObject::SetSelectMode(bool bIsSelected)
{
		if (!ColorConfigurator)
			return;
	
		ColorConfigurator->SetSelectMode(bIsSelected);
}

// マテリアルに色を適用（外部から手動適用する用）
void AColorReactiveObject::ApplyColorToMaterial(FLinearColor InColor)
{
	if (ColorConfigurator == nullptr)
		return;

	ColorConfigurator->ApplyColorToMaterial(InColor);
}

void AColorReactiveObject::ChangeLock(bool b)
{
	if (ColorConfigurator == nullptr)
		return;
	ColorConfigurator->ChangeLock(b);
}

bool AColorReactiveObject::IsColorChange() const
{
	return ColorConfigurator&&ColorConfigurator->IsColorChange();
}

bool AColorReactiveObject::IsChangeable() const
{
	return ColorConfigurator&& ColorConfigurator->IsChangeable();
}

bool AColorReactiveObject::IsColorModifiable() const
{
	if (ColorConfigurator == nullptr)
		return false;
	return ColorConfigurator->IsColorModifiable();
}

bool AColorReactiveObject::IsColorMuch() const
{
	return ColorConfigurator && ColorConfigurator->IsColorMuch();
}

FName AColorReactiveObject::GetColorEventID() const
{
	if (ColorConfigurator == nullptr)
		return " ";

	return ColorConfigurator->GetColorEventID();
}

