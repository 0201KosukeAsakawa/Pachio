#include "Components/Color/ObjectColorComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"

// ====================================================================
// オブジェクトの色管理を担当するコンポーネント
// - カラーマネージャーとの連携
// - 色の適用とマッチング判定
// ====================================================================

// =======================
// 定数
// =======================
namespace
{
    /** カスタムデプスステンシル値 */
    static constexpr int32 CUSTOM_DEPTH_STENCIL_VALUE = 10;

    /** マテリアルスロットインデックス */
    static constexpr int32 MATERIAL_SLOT_INDEX = 0;
}
    

// =======================
// コンストラクタ
// =======================

UObjectColorComponent::UObjectColorComponent()
    : CurrentColor(FLinearColor::White)      // 現在の色（初期値: 白）
    , InitialColor(FLinearColor::White)      // 初期色（リセット時に使用）
    , bApplyColorToMaterial(true)            // マテリアルに色を適用するか
    , bEnableColorAction(true)               // 色変更アクションを有効化
    , bEnableBeatEffect(true)                // ビート演出を有効化
    , bUseComplementaryColor(false)          // 補色を使用するか
    , bColorMatched(false)                   // 色が一致しているか
    , bSelected(false)                       // 選択されているか
    , bColorChangeable(false)                // 色変更が可能か
    , bInitialized(false)                    //初期化済みであるか
    
{
}

// =======================
// 初期化フロー
// =======================

// =======================
// 自動初期化フック
// =======================

void UObjectColorComponent::OnRegister()
{
    Super::OnRegister();
}

void UObjectColorComponent::BeginPlay()
{
    Super::BeginPlay();

    // OnRegisterがスキップされるケース用バックアップ
    if (!bInitialized)
    {
        Initialize();
        bInitialized = true;

        UE_LOG(LogTemp, Log, TEXT("[%s] ColorComponent auto-initialized on BeginPlay."),
            *GetOwner()->GetName());
    }
}

#if WITH_EDITOR
void UObjectColorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (!bInitialized)
    {
        Initialize();
        bInitialized = true;
    }
}
#endif

/**
 * コンポーネント全体の初期化
 * 各種マネージャーへの登録とイベントバインドを行う
 */
void UObjectColorComponent::Initialize()
{
    if (bInitialized)
        return;
    InitializeColorLogic();      // 色ロジックの初期化
    RegisterToColorManager();    // カラーマネージャーへの登録
    SetupMaterial();             // マテリアルの初期設定
}

void UObjectColorComponent::ApplyColorWithMatching(const FLinearColor& NewColor)
{
    SetColor(NewColor);
}

/**
 * 色ロジックの初期化
 * ColorReactiveComponentを生成し、初期色を設定する
 */
void UObjectColorComponent::InitializeColorLogic()
{
    // ReactiveComponentClassが設定されていない場合は警告
    if (!ReactiveComponentClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ReactiveComponentClass is not set in %s"),
            *GetOwner()->GetName());
        return;
    }

    // ColorReactiveコンポーネントを動的に生成
    ColorReactive = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);

    // カラーマネージャーから初期色を取得
    if (ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld()))
    {
        if (UColorManager* ColorManager = LevelManager->GetColorManager())
        {
            InitialColor = ColorManager->GetEffectColor(ColorCategory);
        }
    }

    CurrentColor = InitialColor;

    // ColorReactiveの生成確認
    if (!ColorReactive)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ColorReactive component in %s"),
            *GetOwner()->GetName());
        return;
    }

    // コンポーネントとして登録・有効化
    ColorReactive->RegisterComponent();
    ColorReactive->Activate(true);

    // ColorReactiveの初期化
    // 引数: 初期色, 色を変数として扱うか, オーナーアクター
    ColorReactive->Initialize(InitialColor, true, GetOwner());
    ColorReactive->SetEffectType(ColorCategory);
    ColorReactive->SetupNiagaraActors(NiagaraActors);

    UE_LOG(LogTemp, Log, TEXT("ColorLogic initialized for %s (Effect: %d, Color: R=%.2f G=%.2f B=%.2f)"),
        *GetOwner()->GetName(),
        static_cast<int32>(ColorCategory),
        InitialColor.R, InitialColor.G, InitialColor.B);
}

/**
 * カラーマネージャーへの登録
 * このオブジェクトを色管理対象として登録する
 */
void UObjectColorComponent::RegisterToColorManager()
{
    UColorManager* ColorManager = GetColorManager();
    if (!ColorManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorManager not found for %s"),
            *GetOwner()->GetName());
        return;
    }

    // ターゲットタイプを指定して登録
    ColorManager->RegisterTarget(this);
}

/**
 * マテリアルの初期設定
 * カスタムデプスレンダリングとダイナミックマテリアルを設定
 */
void UObjectColorComponent::SetupMaterial()
{
    // マテリアル適用が有効な場合はスキップ
    if (!bApplyColorToMaterial)
    {
        return;
    }

    // カラーマネージャから初期色を再取得（二重チェック）
    const UColorManager* ColorManager = GetColorManager();
    if (ColorManager)
    {
        InitialColor = ColorManager->GetEffectColor(ColorCategory);
    }

    // メッシュコンポーネントを取得
    UStaticMeshComponent* Mesh = GetMeshComponent();
    if (!Mesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh component not found for %s"),
            *GetOwner()->GetName());
        return;
    }

    // カスタムデプスレンダリングを有効化（アウトライン表示などに使用）
    Mesh->SetRenderCustomDepth(true);
    Mesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_VALUE);

    ApplyColorToMaterial(InitialColor);
}
// =======================
// 色操作API
// =======================

/**
 * 色を設定し、マテリアルとエフェクトに反映
 *
 * @param NewColor 新しい色
 */
void UObjectColorComponent::SetColor(const FLinearColor& NewColor)
{
    // カラーマッチング処理を実行
    ProcessColorMatching(NewColor);
    if (!bColorChangeable)
        return;

    CurrentColor = NewColor;

    // ColorReactiveが無効な場合は早期リターン
    if (ColorReactive == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactive is null in SetColor for %s"),
            *GetOwner()->GetName());
        return;
    }

    // マテリアルへ色を反映
    ApplyColorToMaterial(CurrentColor);


    // エフェクトタイプを更新（最適なエフェクトに切り替え）
    ColorReactive->SetEffectType(UColorUtilityLibrary::GetNearestPrimaryColor(CurrentColor));
}


void UObjectColorComponent::ResetColor()
{
    // 初期色で SetColor を呼び出し
    SetColor(InitialColor);
}

/**
 * 内部的に現在の色のみを更新
 * マテリアルやエフェクトには反映しない（軽量な更新用）
 *
 * @param NewColor 新しい色
 */
void UObjectColorComponent::SetCurrentColorOnly(const FLinearColor& NewColor)
{
    CurrentColor = NewColor;
}

/**
 * 色マッチング処理を実行
 * ワールド色と現在色を比較し、一致判定を行う
 *
 * @param NewColor 新しく設定された色
 * @param MatchResult エフェクトマッチング結果
 */
void UObjectColorComponent::ProcessColorMatching(const FLinearColor& NewColor)
{
    // 色アクションが無効、またはColorReactiveが無効な場合はスキップ
    if (!bEnableColorAction || !ColorReactive)
    {
        return;
    }

    // カラーマネージャーを取得
    const UColorManager* ColorManager = GetColorManager();
    if (!ColorManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorManager not found in ProcessColorMatching for %s"),
            *GetOwner()->GetName());
        return;
    }

    // ワールド色（フィルター色）を取得
    const FLinearColor WorldColor = ColorManager->GetWorldColor();

    // 色変数モード: ワールド色を直接マテリアルに適用
    // このモードでは、オブジェクトの色がワールド色に追従する
    if (bColorChangeable)
    {
        ColorReactive->ApplyColorToMaterial(WorldColor);
        CurrentColor = WorldColor;  // 内部状態も更新
    }

    // 色一致判定を実行（色相角度ベース）
    bColorMatched = UColorUtilityLibrary::IsHueSimilar(InitialColor, WorldColor);

    // 判定結果に応じてColorReactiveにコールバック
    if (!bColorMatched)
    {
        // 色が不一致の場合
        ColorReactive->OnColorMismatched(WorldColor);
    }
    else
    {
        // 色が一致した場合
        ColorReactive->OnColorMatched(WorldColor);
    }

    OnColorChanged.Broadcast(UColorUtilityLibrary::GetNearestPrimaryColor(WorldColor));
}

/**
 * マテリアルに色を適用
 * ColorReactiveを通じて実際のマテリアル更新を行う
 *
 * @param Color 適用する色
 */
void UObjectColorComponent::ApplyColorToMaterial(const FLinearColor& Color)
{
    if (ColorReactive)
    {
        ColorReactive->ApplyColorToMaterial(Color);
    }
}

// =======================
// 状態の取得と設定
// =======================

/**
 * 色の一致状態を設定
 *
 * @param bMatched 一致しているか
 */
void UObjectColorComponent::SetColorMatched(bool bMatched)
{
    bColorMatched = bMatched;
}

/**
 * 選択状態を設定
 * 選択時はエミッシブ効果などが適用される
 *
 * @param bInSelected 選択されているか
 */
void UObjectColorComponent::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;

    // ColorReactiveにも選択状態を通知
    if (ColorReactive)
    {
        ColorReactive->SetSelectMode(bSelected);
    }
}

/**
 * 非表示状態かを取得
 *
 * @return 非表示状態の場合true
 */
bool UObjectColorComponent::IsHidden() const
{
    return ColorReactive && ColorReactive->IsHidden();
}

// =======================
// 色判定API（UColorUtilityLibraryに委譲）
// =======================

/**
 * 初期色から変更されているかを判定
 *
 * @return 変更されている場合true
 */
bool UObjectColorComponent::HasColorChanged(const float Tolerance) const
{
    // 現在色と初期色を比較
    return HasColorChanged(InitialColor,Tolerance);
}

/**
 * 指定色と現在の色が変更されているかを判定
 *
 * @param CompareColor 比較する色
 * @return 変更されている場合true
 */
bool UObjectColorComponent::HasColorChanged(const FLinearColor& CompareColor, float Tolerance) const
{
    return UColorUtilityLibrary::GetHueAngleDistance(CurrentColor, CompareColor) <= Tolerance;
}

// =======================
// ヘルパー関数
// =======================

/**
 * SkeletalMeshComponentを取得
 * オーナーアクターから"Mesh"という名前のコンポーネントを検索
 *
 * @return SkeletalMeshComponent（見つからない場合はnullptr）
 */
UStaticMeshComponent* UObjectColorComponent::GetMeshComponent() const
{
    /*return UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(
        GetOwner(),
        TEXT("Mesh")
    );*/
    return GetOwner()->GetComponentByClass<UStaticMeshComponent>();
}

/**
 * レベルマネージャーを取得
 *
 * @return ALevelManager（見つからない場合はnullptr）
 */
ALevelManager* UObjectColorComponent::GetLevelManager() const
{
    return ALevelManager::GetInstance(GetWorld());
}

/**
 * カラーマネージャーを取得
 * レベルマネージャーを経由して取得
 *
 * @return UColorManager（見つからない場合はnullptr）
 */
UColorManager* UObjectColorComponent::GetColorManager() const
{
    const ALevelManager* LevelManager = GetLevelManager();
    return LevelManager ? LevelManager->GetColorManager() : nullptr;
}