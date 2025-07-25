#include "Objects/ColorReactiveObject.h"
#include "Components/ColorReactiveComponent.h"
#include "Components/BeatScalerComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"

// コンストラクタ：Tick はデフォルトで無効（基本的にリアルタイム更新不要）
AColorReactiveObject::AColorReactiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
	BeatScalerComponent = CreateDefaultSubobject<UBeatScalerComponent>(TEXT("BeatScalerComponent"));
}

// BeginPlay（ゲーム開始時）に初期化処理を実行
void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	Init();
	// 初期化時（BeginPlayなど）
	if (USoundManager* soundManager = Cast<USoundManager>(ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetObject()))
	{
		soundManager->OnBeatDetected.AddDynamic(this, &AColorReactiveObject::PlayBeatAnimation);
	}
}

// 色反応オブジェクトの初期化処理
void AColorReactiveObject::Init()
{
	InitializeColorLogic();    // 色反応コンポーネントの生成・設定
	RegisterToColorManager(); // カラーマネージャーへの登録
	SetupMaterial();          // マテリアルとステンシル値の設定

}

// 色反応ロジックの初期化（UColorReactiveComponentの生成）
void AColorReactiveObject::InitializeColorLogic()
{
	if (ReactiveComponentClass == nullptr)
		return;
	CurrentColor = StartColor;
	// 指定されたクラスからインスタンスを生成
	ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (ColorReactiveComponent == nullptr)
		return;

	// コンポーネントの登録とアクティベート
	ColorReactiveComponent->RegisterComponent();
	ColorReactiveComponent->Activate(true);
	ColorReactiveComponent->SetMyColor(StartColor);

	// StaticMesh にバインド（色の反応対象メッシュ取得）
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	ColorReactiveComponent->Init(Mesh); // メッシュを登録して初期化
}

// レベル上のカラーマネージャーに自身を登録
void AColorReactiveObject::RegisterToColorManager()
{
	ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
	if (LevelManager == nullptr)
		return;

	UColorManager* ColorManager = LevelManager->GetColorManager();
	if (ColorManager == nullptr)
		return;

	// ターゲット種別とともに自分を登録
	ColorManager->RegisterTarget(ColorTargetType, this);
}

// マテリアルとカスタムデプス設定
void AColorReactiveObject::SetupMaterial()
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	// 輪郭描画用のデプスステンシル設定
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(10);

	// ダイナミックマテリアル作成とベース色の設定
	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial == nullptr || !bSetColor)
		return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), StartColor);
}

void AColorReactiveObject::PlayBeatAnimation()
{
	if (BeatScalerComponent && bPlayBeat)
		BeatScalerComponent->PlayBeat();
}

// 色アクション実行時の処理（デフォルト実装）
void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (!bPlayColorAction ||ColorReactiveComponent == nullptr)
		return;
	if (bColorVariable)
		ApplyColorToMaterial(NewColor);

	// 入力色と一致するかチェック（結果は bColorMuch に保持）
	bColorMuch = ColorReactiveComponent->CheckColorMatch(NewColor, buseComplementaryColor);
}

void AColorReactiveObject::SetColor(FLinearColor newColor)
{
	CurrentColor = newColor;
	ApplyColorToMaterial(CurrentColor);
	ColorReactiveComponent->SetMyColor(CurrentColor);
	ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
	if (LevelManager == nullptr)
		return;

	UColorManager* ColorManager = LevelManager->GetColorManager();
	if (ColorManager == nullptr)
		return;
	ColorAction(ColorManager->GetWorldColor());
}

void AColorReactiveObject::ResetColor()
{
	SetColor(StartColor);
}

bool AColorReactiveObject::IsColorChange() const
{
	if (!ColorReactiveComponent)
		return false;

	return ColorReactiveComponent->IsColorMatch(StartColor);
}

void AColorReactiveObject::SetSelectMode(bool bIsSelected)
{
	if (!ColorReactiveComponent)
		return;

	return ColorReactiveComponent->SetSelectMode(bIsSelected);
}

// マテリアルに色を適用（外部から手動適用する用）
void AColorReactiveObject::ApplyColorToMaterial(FLinearColor InColor)
{
	ColorReactiveComponent->ApplyColorToMaterial(InColor);
}