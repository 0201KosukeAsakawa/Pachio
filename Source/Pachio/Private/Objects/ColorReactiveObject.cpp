#include "Objects/ColorReactiveObject.h"
#include "Components/ColorReactiveComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "FunctionLibrary.h"

// コンストラクタ：Tick はデフォルトで無効（基本的にリアルタイム更新不要）
AColorReactiveObject::AColorReactiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

// BeginPlay（ゲーム開始時）に初期化処理を実行
void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	Init();
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

	// 指定されたクラスからインスタンスを生成
	ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (ColorReactiveComponent == nullptr)
		return;

	// コンポーネントの登録とアクティベート
	ColorReactiveComponent->RegisterComponent();
	ColorReactiveComponent->Activate(true);
	ColorReactiveComponent->SetMyColor(Color);

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
	if (DynMaterial == nullptr)
		return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), Color);
}

// 色アクション実行時の処理（デフォルト実装）
void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (bColorLock || ColorReactiveComponent == nullptr)
		return;

	// 入力色と一致するかチェック（結果は bColorMuch に保持）
	bColorMuch = ColorReactiveComponent->CheckColorMatch(NewColor);
}

// マテリアルに色を適用（外部から手動適用する用）
void AColorReactiveObject::ApplyColorToMaterial(FLinearColor InColor)
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (!Mesh) return;

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMaterial) return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), InColor);
}

// -------------------------
// RGB → HSL 変換関数
FHSLColor RGBToHSL(const FLinearColor& Color)
{
	float R = Color.R;
	float G = Color.G;
	float B = Color.B;

	float Max = FMath::Max3(R, G, B);
	float Min = FMath::Min3(R, G, B);
	float Delta = Max - Min;

	FHSLColor HSL;
	HSL.L = (Max + Min) / 2.0f;

	if (Delta == 0)
	{
		HSL.H = 0.0f;
		HSL.S = 0.0f;
	}
	else
	{
		HSL.S = (HSL.L < 0.5f) ? (Delta / (Max + Min)) : (Delta / (2.0f - Max - Min));

		if (Max == R)
			HSL.H = (G - B) / Delta + (G < B ? 6.0f : 0.0f);
		else if (Max == G)
			HSL.H = (B - R) / Delta + 2.0f;
		else
			HSL.H = (R - G) / Delta + 4.0f;

		HSL.H /= 6.0f;
	}
	return HSL;
}

// HSL → RGB 変換関数
FLinearColor HSLToRGB(const FHSLColor& HSL)
{
	float R, G, B;

	if (HSL.S == 0)
	{
		R = G = B = HSL.L; // 無彩色
	}
	else
	{
		auto HueToRGB = [](float p, float q, float t) -> float
			{
				if (t < 0.0f) t += 1.0f;
				if (t > 1.0f) t -= 1.0f;
				if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
				if (t < 1.0f / 2.0f) return q;
				if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
				return p;
			};

		float q = (HSL.L < 0.5f) ? (HSL.L * (1 + HSL.S)) : (HSL.L + HSL.S - HSL.L * HSL.S);
		float p = 2 * HSL.L - q;

		R = HueToRGB(p, q, HSL.H + 1.0f / 3.0f);
		G = HueToRGB(p, q, HSL.H);
		B = HueToRGB(p, q, HSL.H - 1.0f / 3.0f);
	}

	return FLinearColor(R, G, B, 1.0f);
}

FLinearColor AColorReactiveObject::GetComplementaryColor(const FLinearColor& InColor)
{
	// RGB → HSL に変換
	FHSLColor HSL = RGBToHSL(InColor);

	// パステル調に補正（元の色も淡くする）
	HSL.S = 0.3f;
	HSL.L = 0.75f;

	// 補色（色相を180度反転）
	HSL.H += 0.5f;
	if (HSL.H > 1.0f) HSL.H -= 1.0f;

	// HSL → RGB に変換して返す
	return HSLToRGB(HSL);
}
