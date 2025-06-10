#include "Manager/ColorManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Interface/ColorFilterInterface.h"
#include "Components/ColorControllerComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// 色とバフ効果の対応を管理するクラス
void UColorManager::Init()
{
    // 対象オブジェクト（色を受けるもの）を初期化
    InitializeTargets();

    // プレイヤーのコントローラーから色変更イベントを受け取る
    BindController();

    // ポストプロセスボリュームとマテリアル初期化（視覚効果用）
    InitializePostEffect();

    // バフ効果とその基準色のマッピングを定義
    EffectColorMap = {
        { EBuffEffect::JumpBoost,  FLinearColor::Green },
        { EBuffEffect::SpeedBoost, FLinearColor::Blue},
        { EBuffEffect::Shield,     FLinearColor::Red }
    };
}

// 色を反映し、ターゲットに通知する
void UColorManager::ApplyColor(FLinearColor NewColor, EColorTargetType Mode)
{
    switch (Mode)
    {
    case EColorTargetType::Layer:
        if (PostProcessMID)
        {
            // ポストプロセスマテリアルに色を適用
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        // 常時反応するターゲット（例：UIなど）に通知
        NotifyTargets(EColorTargetType::Responders, NewColor);
        break;

    case EColorTargetType::Object:
    case EColorTargetType::Background:
        // 指定されたモードのターゲットに通知
        NotifyTargets(Mode, NewColor);
        break;

    default:
        break;
    }
}

// 色変化に反応するターゲットを登録
void UColorManager::RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target)
{
    if (!Target) return;

    FColorTargetInstanceArray& TargetArray = ColorResponseTargets.FindOrAdd(Mode);
    if (!TargetArray.Instances.Contains(Target))
    {
        TargetArray.Instances.Add(Target);
    }
}

// 登録されたターゲットクラスをインスタンス化して準備
void UColorManager::InitializeTargets()
{
    ColorResponseTargets.Empty();

    for (auto& Pair : ColorTargetsClass)
    {
        EColorTargetType ModeKey = Pair.Key;
        const FColorTargetArray& ClassArray = Pair.Value;

        FColorTargetInstanceArray& InstanceArray = ColorResponseTargets.FindOrAdd(ModeKey);
        InstanceArray.Instances.Empty();

        for (TSubclassOf<UObject> TargetClass : ClassArray.Targets)
        {
            if (TargetClass)
            {
                UObject* NewObj = NewObject<UObject>(this, TargetClass);
                if (NewObj && NewObj->GetClass()->ImplementsInterface(UColorReactiveInterface::StaticClass()))
                {
                    TScriptInterface<IColorReactiveInterface> InterfaceObj;
                    InterfaceObj.SetObject(NewObj);
                    InterfaceObj.SetInterface(Cast<IColorReactiveInterface>(NewObj));
                    InstanceArray.Instances.Add(InterfaceObj);
                }
            }
        }
    }
}

// プレイヤーの色コントローラーとイベント接続
void UColorManager::BindController()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UColorControllerComponent* ColorController = PlayerPawn->FindComponentByClass<UColorControllerComponent>();
        if (ColorController && !ColorController->OnColorChanged.IsAlreadyBound(this, &UColorManager::ApplyColor))
        {
            // 色変更イベントにバインド
            ColorController->OnColorChanged.AddDynamic(this, &UColorManager::ApplyColor);
        }
    }
}

// ポストプロセス用マテリアルの初期化（ビジュアルフィルター表示などに使用）
void UColorManager::InitializePostEffect()
{
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.Num() < 0)
        return;

    APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(FoundVolumes[0]);

    if (PostProcessVolume && PostProcessMaterial)
    {
        // マテリアルインスタンスを作成しポストプロセスに適用
        PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMID));
    }
}

// 指定モードの全ターゲットに色を通知
void UColorManager::NotifyTargets(EColorTargetType Mode, const FLinearColor& Color)
{
    if (FColorTargetInstanceArray* TargetArray = ColorResponseTargets.Find(Mode))
    {
        for (const TScriptInterface<IColorReactiveInterface>& Target : TargetArray->Instances)
        {
            if (Target)
            {
                // ターゲットの反応関数を呼び出す
                Target->ColorAction(Color);
            }
        }
    }
}
float UColorManager::GetColorDistanceRGB(const FLinearColor& A, const FLinearColor& B)
{
    return FMath::Sqrt(
        FMath::Square(A.R - B.R) +
        FMath::Square(A.G - B.G) +
        FMath::Square(A.B - B.B)
    );
}

FEffectMatchResult UColorManager::GetClosestEffectByHue(const FLinearColor& InputColor)
{
    FEffectMatchResult result;

    float MinDistance = TNumericLimits<float>::Max();
    float MaxPossibleDistance = FMath::Sqrt(3.0f); // RGB距離の最大値（(1,1,1)と(0,0,0)の距離）

    EBuffEffect ClosestEffect = EBuffEffect::None;

    for (const auto& Elem : EffectColorMap)
    {
        float Distance = GetColorDistanceRGB(InputColor, Elem.Value);

        UE_LOG(LogTemp, Log, TEXT("Comparing with %d: RGB Distance = %.4f"),
            static_cast<int32>(Elem.Key), Distance);

        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestEffect = Elem.Key;
        }
    }

    // 距離が最大値に近いほど弱く、0に近いほど強い（逆スケール）
    float StrengthRatio = 1.0f - (MinDistance / MaxPossibleDistance);
    StrengthRatio = FMath::Clamp(StrengthRatio, 0.0f, 1.0f); // 念のため

    // ログ出力
    UE_LOG(LogTemp, Log, TEXT("ClosestEffect: %d (RGB Distance = %.4f, StrengthRatio = %.2f)"),
        static_cast<int32>(ClosestEffect), MinDistance, StrengthRatio);

    // 結果設定
    result.ClosestEffect = ClosestEffect;
    result.Distance = MinDistance;
    result.StrengthRatio = StrengthRatio;

    return result;
}