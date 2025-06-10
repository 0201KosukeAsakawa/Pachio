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
void UColorManager::RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorFilterInterface> Target)
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
                if (NewObj && NewObj->GetClass()->ImplementsInterface(UColorFilterInterface::StaticClass()))
                {
                    TScriptInterface<IColorFilterInterface> InterfaceObj;
                    InterfaceObj.SetObject(NewObj);
                    InterfaceObj.SetInterface(Cast<IColorFilterInterface>(NewObj));
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
        for (const TScriptInterface<IColorFilterInterface>& Target : TargetArray->Instances)
        {
            if (Target)
            {
                // ターゲットの反応関数を呼び出す
                Target->ColorAction(Color);
            }
        }
    }
}

// HSV色空間上の色相（Hue）距離を計算（円環状のため180°が最大距離）
float UColorManager::GetHueDistance(float HueA, float HueB)
{
    float Diff = FMath::Abs(HueA - HueB);
    return FMath::Min(Diff, 360.0f - Diff);  // 例: 5°と355°は10°差
}

// 入力色から最も近いバフ効果を色相で判定し、強度を算出
FEffectMatchResult UColorManager::GetClosestEffectByHue(const FLinearColor& InputColor)
{
    float InputHue = InputColor.LinearRGBToHSV().R * 360.0f;

    FEffectMatchResult Result;
    Result.Distance = 9999.0f;

    for (const auto& Pair : EffectColorMap)
    {
        float TargetHue = Pair.Value.LinearRGBToHSV().R * 360.0f;
        float HueDist = GetHueDistance(InputHue, TargetHue);

        if (HueDist < Result.Distance)
        {
            Result.ClosestEffect = Pair.Key;
            Result.Distance = HueDist;
        }
    }

    // 最大180°に対しての距離で強さを算出（近いほど強い）
    Result.StrengthRatio = FMath::Clamp(1.0f - (Result.Distance / 180.0f), 0.0f, 1.0f);

    return Result;
}
