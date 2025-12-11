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

    static constexpr float CHANGE_HITCOLOR = 2.f;
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
    , HitTimer(0.f)
{
    PrimaryComponentTick.bCanEverTick = true;
}

// =======================
// 初期化フロー
// =======================

// =======================
// 自動初期化フック
// =======================
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

void UObjectColorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    LastPaintTime += GetWorld()->DeltaTimeSeconds;
    if (bIsPlayedPaint)
    {
        if (LastPaintTime > 0.5f)
        {
            bIsPlayedPaint = false;
            HitTimer = 0.f;
        }
    }

    if (!bIsPainting)
    {
        return;
    }
    if (LastPaintTime < CHANGE_HITCOLOR)
        return;
    HitTimer -= GetWorld()->DeltaTimeSeconds;
    // 補間割合（0→1）
    float Ratio = FMath::Clamp(HitTimer / CHANGE_HITCOLOR, 0.0f, 1.0f);

    // Hue 補間ベースで色を更新
    HitColor = UColorUtilityLibrary::LerpHue(HitColor, CurrentColor, Ratio);

    // 補間中は MaterialAlpha で反映
    ApplyColorToMaterialAlpha(1-Ratio, CurrentColor);
    //ApplyColorToMaterial(HitColor);

    if (Ratio >= 1)
    {
        LastPaintTime = 0;
        HitTimer = 0;
        bIsPainting = false;
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

    UStaticMeshComponent* MeshComp = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
    if (MeshComp)
    {
        // ダイナミックマテリアルを生成
        constexpr int32 MaterialSlotIndex = 0;
        DynMesh = MeshComp->CreateAndSetMaterialInstanceDynamic(MaterialSlotIndex);
    }

    InitializeColorLogic();      // 色ロジックの初期化
    RegisterToColorManager();    // カラーマネージャーへの登録
    SetupMaterial();             // マテリアルの初期設定
}

void UObjectColorComponent::ApplyColorWithMatching(const FLinearColor& NewColor)
{
    // 色変更が開始された瞬間の処理
    bool bNear = UColorUtilityLibrary::IsHueSimilar(HitColor, NewColor, 1);
    if (!bNear)
    {
        // 初回の変更時にリセット
        if (LastColor != NewColor)
        {
            LastColor = NewColor;
            StartColor = HitColor;  // 補間開始位置を確保
        }
    }

    // タイマー進行
    HitTimer += GetWorld()->DeltaTimeSeconds;
    bIsPainting = true;
    LastPaintTime = 0;
    // 補間割合（0→1）
    float Ratio = FMath::Clamp(HitTimer / CHANGE_HITCOLOR, 0.0f, 1.0f);

    // Hue 補間ベースで色を更新
    HitColor = UColorUtilityLibrary::LerpHue(StartColor, NewColor, Ratio);
    if (!bIsPlayedPaint && !bNear)
    {
        // 補間中は MaterialAlpha で反映
        ApplyColorToMaterialAlpha(1.0f - Ratio, HitColor);
    }
    // 補間が完了したら最終色をセット
    if (Ratio >= 1.0f)
    {
        SetColor(NewColor);
        bIsPainting = false;
        bIsPlayedPaint = true;
    }
}

/**
 * 色ロジックの初期化
 * ColorReactiveComponentを生成し、初期色を設定する
 */
void UObjectColorComponent::InitializeColorLogic()
{
    // カラーマネージャーから初期色を取得
    if (ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld()))
    {
        if (UColorManager* ColorManager = LevelManager->GetColorManager())
        {
            InitialColor = ColorManager->GetEffectColor(ColorCategory);
        }
    }

    CurrentColor = InitialColor;

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
    if (!bColorChangeable)
        return;

    CurrentColor = HitColor;
    // マテリアルへ色を反映
    ApplyColorToMaterial(CurrentColor);
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
 * 非表示状態かを取得
 *
 * @return 非表示状態の場合true
 */
bool UObjectColorComponent::IsHidden() const
{
    return false;//ColorReactive/* && ColorReactive->IsHidden()*/;
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
// 色の適用
// =======================
/**
 * マテリアルに色を適用
 * ColorReactiveを通じて実際のマテリアル更新を行う
 *
 * @param Color 適用する色
 */
void UObjectColorComponent::ApplyColorToMaterial(const FLinearColor& InColor)
{
    if (!DynMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: DynMesh is null in ApplyColorToMaterial"));
        return;
    }

    CurrentColor = InColor;
    DynMesh->SetVectorParameterValue(FName("BaseColor"), InColor);
}

void UObjectColorComponent::ApplyColorToMaterialAlpha(const float Alpha, const FLinearColor& InColor)
{
    if (!DynMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: DynMesh is null in ApplyColorToMaterial"));
        return;
    }

    DynMesh->SetVectorParameterValue(FName("Param"), InColor);
    DynMesh->SetScalarParameterValue(FName("Alpha"), Alpha);
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

