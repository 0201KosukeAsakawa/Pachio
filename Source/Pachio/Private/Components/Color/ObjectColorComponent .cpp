#include "Components/Color/ObjectColorComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// =======================
// 定数
// =======================
namespace
{
    /** カスタムデプスステンシル値 */
    static constexpr int32 CUSTOM_DEPTH_STENCIL_VALUE = 10;

    /** マテリアルスロットインデックス */
    static constexpr int32 MATERIAL_SLOT_INDEX = 0;

    /** 色相変化速度（度/秒） */
    static constexpr float HUE_CHANGE_SPEED = 30.0f;

    /** 色変更の持続時間（秒） */
    static constexpr float COLOR_CHANGE_DURATION = 2.f;

    /** 彩度がこの値以下の場合、無彩色とみなす */
    static constexpr float ACHROMATIC_SATURATION_THRESHOLD = 0.01f;
}


// =======================
// コンストラクタ
// =======================

UObjectColorComponent::UObjectColorComponent()
    : CurrentColor(FLinearColor::White)      // 現在の色（初期値: 白）
    , HitColor(FLinearColor::White)          // ヒット時の色
    , InitialColor(FLinearColor::White)      // 初期色（リセット時に使用）
    , TargetColor(FLinearColor::White)       // 目標色（Tick内で徐々に近づく）
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

    // 目標色への段階的変化（SetTargetColor用）
    if (bHasTargetColor && bColorChangeable)
    {
        UpdateColorGradually(DeltaTime);
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
        // ダイナミックマテリアルを生成（メッシュ色変更モードの場合のみ）
        if (ColorChangeMode != EColorChangeMode::NiagaraOnly)
        {
            constexpr int32 MaterialSlotIndex = 0;
            DynMesh = MeshComp->CreateAndSetMaterialInstanceDynamic(MaterialSlotIndex);
        }
    }

    // ナイアガラコンポーネントの初期化（ナイアガラモードの場合）
    if (ColorChangeMode != EColorChangeMode::MeshColorOnly && NiagaraSystem)
    {
        NiagaraComponent = NewObject<UNiagaraComponent>(GetOwner());
        if (NiagaraComponent)
        {
            NiagaraComponent->SetAsset(NiagaraSystem);
            NiagaraComponent->RegisterComponent();
            NiagaraComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
            NiagaraComponent->SetAutoActivate(false);

            UE_LOG(LogTemp, Log, TEXT("[%s] NiagaraComponent initialized for mode: %d"),
                *GetOwner()->GetName(), static_cast<int32>(ColorChangeMode));
        }
    }

    InitializeColorLogic();      // 色ロジックの初期化
    RegisterToColorManager();    // カラーマネージャーへの登録
    SetupMaterial();             // マテリアルの初期設定
}

/**
 * 従来の色塗り方式（即座に変化）
 * PaintHitObjectなどから呼び出される
 */
void UObjectColorComponent::ActivateDirect(const FLinearColor& NewColor)
{
    // モードに応じた処理を実行
    switch (ColorChangeMode)
    {
    case EColorChangeMode::MeshColorOnly:
        // メッシュの色のみを変更
        ApplyColorToMaterial(NewColor);
        break;

    case EColorChangeMode::NiagaraOnly:
        // ナイアガラエフェクトのみを起動
        ActivateNiagaraEffect();
        break;

    case EColorChangeMode::MeshColorAndNiagara:
        // 両方を実行
        ApplyColorToMaterial(NewColor);
        ActivateNiagaraEffect();
        break;
    }
}

void UObjectColorComponent::SetTargetColor(const FLinearColor& NewColor, float Duration)
{
    if (!bColorChangeable)
        return;

    TargetColor = NewColor;
    bHasTargetColor = true;
    ColorChangeTimer = 0.f;

    // ★ 追加: 初期フレームで即座に色を適用開始
    if (ColorChangeMode != EColorChangeMode::NiagaraOnly)
    {
        // 最初の1フレーム分の更新を即座に実行
        UpdateColorGradually(0.016f); // 約60FPSの1フレーム分
    }

    UE_LOG(LogTemp, Log, TEXT("[%s] Target color set for %.1f seconds: R=%.2f G=%.2f B=%.2f"),
        *GetOwner()->GetName(),
        Duration,
        TargetColor.R, TargetColor.G, TargetColor.B);
}

/**
 * Tick内で呼ばれる色更新処理（30度/秒で段階的に変化、時間制限付き）
 * 無彩色から有彩色への変化時は、目標色の色相を直接使用する
 *
 * @param DeltaTime フレーム時間
 */
void UObjectColorComponent::UpdateColorGradually(float DeltaTime)
{
    // 経過時間を更新
    ColorChangeTimer += DeltaTime;

    // ★ 修正: 時間切れチェックを最後に移動
    // まず色を更新してから時間をチェックする

    // 現在の色と目標色のHSLを取得
    FVector currentHSL = UColorUtilityLibrary::GetHSL(CurrentColor);
    FVector targetHSL = UColorUtilityLibrary::GetHSL(TargetColor);

    float newHue = currentHSL.X;
    float deltaHue = 0.0f;

    // 現在の色が無彩色（白・グレー・黒）かチェック
    bool bCurrentIsAchromatic = currentHSL.Y < ACHROMATIC_SATURATION_THRESHOLD;
    bool bTargetIsAchromatic = targetHSL.Y < ACHROMATIC_SATURATION_THRESHOLD;

    if (bCurrentIsAchromatic && !bTargetIsAchromatic)
    {
        // 無彩色から有彩色へ：目標色の色相を直接使用
        newHue = targetHSL.X;

        UE_LOG(LogTemp, Verbose, TEXT("[%s] Transitioning from achromatic to chromatic. Using target hue: %.1f"),
            *GetOwner()->GetName(), targetHSL.X);
    }
    else if (!bCurrentIsAchromatic && !bTargetIsAchromatic)
    {
        // 有彩色から有彩色へ：最短角距離で補間
        deltaHue = targetHSL.X - currentHSL.X;
        deltaHue = FMath::Fmod(deltaHue + 540.0f, 360.0f) - 180.0f;

        // このフレームでの最大変化量（30度/秒）
        float maxHueChangeThisFrame = HUE_CHANGE_SPEED * DeltaTime;
        float hueStep = FMath::Clamp(deltaHue, -maxHueChangeThisFrame, maxHueChangeThisFrame);

        // 新しいHueを計算
        newHue = FMath::Fmod(currentHSL.X + hueStep + 360.0f, 360.0f);
    }

    // SaturationとLightnessも滑らかに補間
    float interpSpeed = 1.0f * DeltaTime;
    float newS = FMath::Lerp(currentHSL.Y, targetHSL.Y, interpSpeed);
    float newL = FMath::Lerp(currentHSL.Z, targetHSL.Z, interpSpeed);

    // 新しい色を生成して適用
    FLinearColor newColor = UColorUtilityLibrary::FromHSL(FVector(newHue, newS, newL));
    CurrentColor = newColor;

    // ★ 修正: 必ずマテリアルに適用
    if (ColorChangeMode != EColorChangeMode::NiagaraOnly)
    {
        ApplyColorToMaterial(CurrentColor);
    }

    // 目標色に到達したかチェック
    float remainingHueDiff = FMath::Abs(FMath::Fmod(targetHSL.X - newHue + 540.0f, 360.0f) - 180.0f);

    // 無彩色の場合は色相差を無視
    if (bTargetIsAchromatic)
    {
        remainingHueDiff = 0.0f;
    }

    bool bReachedTarget = (remainingHueDiff < 1.0f &&
        FMath::Abs(targetHSL.Y - newS) < 0.05f &&
        FMath::Abs(targetHSL.Z - newL) < 0.05f);

    // ★ 修正: 到達判定または時間切れで終了
    if (bReachedTarget || ColorChangeTimer >= COLOR_CHANGE_DURATION)
    {
        // 最終的な色を確実に適用
        if (ColorChangeMode != EColorChangeMode::NiagaraOnly)
        {
            ApplyColorToMaterial(bReachedTarget ? TargetColor : CurrentColor);
        }

        bHasTargetColor = false;

        if (UColorUtilityLibrary::IsHueSimilar(CurrentColor, InitialColor, FVector(0.05, 0.05, 0.05)))
        {
            ActivateDirect(CurrentColor);
        }

        UE_LOG(LogTemp, Log, TEXT("[%s] Color change finished: %s in %.2f seconds"),
            *GetOwner()->GetName(),
            bReachedTarget ? TEXT("Reached") : TEXT("Timeout"),
            ColorChangeTimer);
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
    if (!bApplyColorToMaterial || ColorChangeMode == EColorChangeMode::NiagaraOnly)
    {
        return;
    }

    // メッシュコンポーネントを取得
    UMeshComponent* Mesh = GetMeshComponent();
    if (!Mesh)
    {
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

    // メッシュ色変更モードの場合のみマテリアルへ色を反映
    if (ColorChangeMode != EColorChangeMode::NiagaraOnly)
    {
        ApplyColorToMaterial(CurrentColor);
    }
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

/**
 * ナイアガラエフェクトを起動
 * 設定されたモードに基づいてエフェクトを再生する
 */
void UObjectColorComponent::ActivateNiagaraEffect()
{
    if (!NiagaraComponent || !NiagaraSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Cannot activate Niagara: Component or System is null"),
            *GetOwner()->GetName());
        return;
    }

    // ナイアガラエフェクトを再生
    NiagaraComponent->Activate(true);

    UE_LOG(LogTemp, Log, TEXT("[%s] Niagara effect activated"),
        *GetOwner()->GetName());
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