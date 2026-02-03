#include "Components/Color/ObjectColorComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"

// =======================
// 定数
// =======================
namespace
{
    /** カスタムデプスステンシル値 */
    static constexpr int32 CUSTOM_DEPTH_STENCIL_VALUE = 10;

    /** マテリアルスロットインデックス */
    static constexpr int32 MATERIAL_SLOT_INDEX = 0;

    /** 色変更時間 */
    static constexpr float CHANGE_HITCOLOR = 2.f;

    /** 色相変化速度（度/秒） - 新しい塗り方用 */
    static constexpr float HUE_CHANGE_SPEED = 30.0f;

    /** 色変更の持続時間（秒） */
    static constexpr float COLOR_CHANGE_DURATION = 2.f;
}


// =======================
// コンストラクタ
// =======================

UObjectColorComponent::UObjectColorComponent()
    : CurrentColor(FLinearColor::White)      // 現在の色（初期値: 白）
    , HitColor(FLinearColor::White)          // ヒット時の色
    , InitialColor(FLinearColor::White)      // 初期色（リセット時に使用）
    , TargetColor(FLinearColor::White)       // 目標色（Tick内で徐々に近づく）
    , StartColor(FLinearColor::White)        // 補間開始時の色
    , LastColor(FLinearColor::White)         // 前回の目標色
    , bApplyColorToMaterial(true)            // マテリアルに色を適用するか
    , bEnableColorAction(true)               // 色変更アクションを有効化
    , bUseComplementaryColor(false)          // 補色を使用するか
    , bColorChangeable(true)                 // 色変更が可能か
    , bColorMatched(false)                   // 色が一致しているか
    , bSelected(false)                       // 選択されているか
    , bInitialized(false)                    // 初期化済みであるか
    , bIsPlayedPaint(false)                  // ペイント演出が再生済みか
    , bIsPainting(false)                     // ペイント中か
    , bHasTargetColor(false)                 // 目標色が設定されているか
    , HitTimer(0.f)                          // ヒット時の経過時間
    , LastPaintTime(0.f)                     // 最後のペイント時刻
    , ColorChangeTimer(0.f)                  // 色変更の経過時間
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
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // ===== 新規追加：目標色への段階的変化（SetTargetColor用） =====
    if (bHasTargetColor && bColorChangeable)
    {
        UpdateColorGradually(DeltaTime);
        //ApplyColorToMaterialAlpha(1.0f - FMath::Clamp(ColorChangeTimer / CHANGE_HITCOLOR, 0.0f, 1.0f), HitColor);
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

    UMeshComponent* MeshComp = GetMeshComponent();
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

/**
 * 従来の色塗り方式（90度/秒で変化）
 * PaintHitObjectなどから呼び出される
 */
void UObjectColorComponent::ActivateDirect(const FLinearColor& NewColor)
{

}


void UObjectColorComponent::SetTargetColor(const FLinearColor& NewColor, float Duration)
{
    if (!bColorChangeable)
        return;

    TargetColor = NewColor;
    bHasTargetColor = true;
    ColorChangeTimer = 0.f;
    //ColorChangeDuration = Duration;

    UE_LOG(LogTemp, Log, TEXT("[%s] Target color set for %.1f seconds: R=%.2f G=%.2f B=%.2f"),
        *GetOwner()->GetName(),
        Duration,
        TargetColor.R, TargetColor.G, TargetColor.B);
}

/**
 * Tick内で呼ばれる色更新処理（30度/秒で段階的に変化、時間制限付き）
 * 白などの無彩色から有彩色への変化にも対応
 *
 * @param DeltaTime フレーム時間
 */
void UObjectColorComponent::UpdateColorGradually(float DeltaTime)
{
    // 経過時間を更新
    ColorChangeTimer += DeltaTime;

    // 時間切れチェック
    if (ColorChangeTimer >= COLOR_CHANGE_DURATION)
    {
        bHasTargetColor = false;
        UE_LOG(LogTemp, Log, TEXT("[%s] Color change time expired (%.2f seconds)"),
            *GetOwner()->GetName(), ColorChangeTimer);

        if (UColorUtilityLibrary::IsHueSimilar(CurrentColor, InitialColor))
        {
            ActivateDirect(CurrentColor);
        }
        return;
    }

    // 現在の色と目標色のHSLを取得
    FVector currentHSL = UColorUtilityLibrary::GetHSL(CurrentColor);
    FVector targetHSL = UColorUtilityLibrary::GetHSL(TargetColor);

    // 彩度の閾値（これ以下の場合は無彩色と見なす）
    constexpr float LOW_SATURATION_THRESHOLD = 0.1f;

    float newHue = currentHSL.X;

    // 現在の彩度が低い場合（白や灰色など）
    if (currentHSL.Y < LOW_SATURATION_THRESHOLD)
    {
        // 目標色の色相を直接採用（赤を経由しない）
        newHue = targetHSL.X;
    }
    else
    {
        // 通常の色相変化（最短経路で回転）
        float deltaHue = targetHSL.X - currentHSL.X;
        deltaHue = FMath::Fmod(deltaHue + 540.0f, 360.0f) - 180.0f;

        // このフレームでの最大変化量（30度/秒）
        float maxHueChangeThisFrame = HUE_CHANGE_SPEED * DeltaTime;
        float hueStep = FMath::Clamp(deltaHue, -maxHueChangeThisFrame, maxHueChangeThisFrame);

        newHue = FMath::Fmod(currentHSL.X + hueStep + 360.0f, 360.0f);
    }

    // SaturationとLightnessを滑らかに補間
    float interpSpeed = 1.0f * DeltaTime;
    float newS = FMath::Lerp(currentHSL.Y, targetHSL.Y, interpSpeed);
    float newL = FMath::Lerp(currentHSL.Z, targetHSL.Z, interpSpeed);

    // 新しい色を生成して適用
    FLinearColor newColor = UColorUtilityLibrary::FromHSL(FVector(newHue, newS, newL));
    CurrentColor = newColor;
    ApplyColorToMaterial(CurrentColor);

    // 目標色に到達したかチェック
    // 彩度が低い場合は色相の差を無視
    bool hueMatched = (currentHSL.Y < LOW_SATURATION_THRESHOLD && newS < LOW_SATURATION_THRESHOLD) ||
        FMath::Abs(FMath::Fmod(targetHSL.X - newHue + 540.0f, 360.0f) - 180.0f) < 1.0f;

    if (hueMatched &&
        FMath::Abs(targetHSL.Y - newS) < 0.05f &&
        FMath::Abs(targetHSL.Z - newL) < 0.05f)
    {
        // 完全に目標色に到達
        ApplyColorToMaterial(CurrentColor);
        bHasTargetColor = false;
        if (UColorUtilityLibrary::IsHueSimilar(CurrentColor, InitialColor, FVector(0.05, 0.05, 0.05)))
        {
            ActivateDirect(CurrentColor);
        }
        UE_LOG(LogTemp, Log, TEXT("[%s] Reached target color in %.2f seconds"),
            *GetOwner()->GetName(), ColorChangeTimer);
    }
}

/**
 * 色ロジックの初期化
 * ColorReactiveComponentを生成し、初期色を設定する
 */
void UObjectColorComponent::InitializeColorLogic()
{
    InitialColor = UColorUtilityLibrary::GetCategoryColor(ColorCategory);

    CurrentColor = InitialColor;
    HitColor = InitialColor;
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
        if(GetOwner() != nullptr)
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
    // メッシュコンポーネントを取得
    UMeshComponent* Mesh = GetMeshComponent();
    if (!Mesh)
    {
        if(GetOwner() != nullptr)
        UE_LOG(LogTemp, Warning, TEXT("Mesh component not found for %s"),
            *GetOwner()->GetName());
        return;
    }

    // カスタムデプスレンダリングを有効化（アウトライン表示などに使用）
    Mesh->SetRenderCustomDepth(true);
    Mesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_VALUE);
    FLinearColor materialColor = UColorUtilityLibrary::GetCategoryColor(StartMaterialColorCategory);
    ApplyColorToMaterial(materialColor);
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
 * MeshComponentを取得
 * まずStaticMeshを探し、見つからない場合はSkeletalMeshを探す
 *
 * @return UMeshComponent（見つからない場合はnullptr）
 */
UMeshComponent* UObjectColorComponent::GetMeshComponent() const
{
    if (!GetOwner())
    {
        return nullptr;
    }

    // まずStaticMeshを探す
    if (UStaticMeshComponent* StaticMesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>())
    {
        return StaticMesh;
    }

    // StaticMeshが見つからない場合はSkeletalMeshを探す
    if (USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>())
    {
        return SkeletalMesh;
    }

    // どちらも見つからない
    UE_LOG(LogTemp, Warning, TEXT("ObjectColorComponent: No mesh component found on %s"),
        *GetOwner()->GetName());
    return nullptr;
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