#include "Manager/ColorManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Interface/ColorFilterInterface.h"
#include "Components/ColorControllerComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void UColorManager::InitializeTargets()
{
    ColorResponseTargets.Empty();

    // クラスベースのターゲットをインスタンス化して登録
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

    // アクティブなレイヤーターゲットを初期化
    ActiveLayerTarget = nullptr;

    // プレイヤーのコントローラーコンポーネントに色変更イベントをバインド
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UColorControllerComponent* ColorController = PlayerPawn->FindComponentByClass<UColorControllerComponent>();
        if (ColorController && !ColorController->OnColorChanged.IsAlreadyBound(this, &UColorManager::ApplyColor))
        {
            ColorController->OnColorChanged.AddDynamic(this, &UColorManager::ApplyColor);
        }
    }

    // ポストプロセスボリュームとマテリアルの初期化
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.Num() < 0)
        return;

    APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(FoundVolumes[0]);

    if (PostProcessVolume && PostProcessMaterial)
    {
        PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMID));
    }

}

void UColorManager::ApplyColor(FLinearColor NewColor, EColorTargetType Mode)
{
    switch (Mode)
    {
    case EColorTargetType::Layer:
        if (PostProcessMID)
        {
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        // レスポンダー（常に反応するターゲット）へ通知
        NotifyTargets(EColorTargetType::Responders, NewColor);
        break;

    case EColorTargetType::Object:
    case EColorTargetType::Background:
        // 該当モードのターゲットへ通知
        NotifyTargets(Mode, NewColor);
        break;

    default:
        break;
    }
}

void UColorManager::RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorFilterInterface> Target)
{
    if (!Target) return;

    FColorTargetInstanceArray& TargetArray = ColorResponseTargets.FindOrAdd(Mode);
    if (!TargetArray.Instances.Contains(Target))
    {
        TargetArray.Instances.Add(Target);
    }
}

void UColorManager::NotifyTargets(EColorTargetType Mode, const FLinearColor& Color)
{
    if (FColorTargetInstanceArray* TargetArray = ColorResponseTargets.Find(Mode))
    {
        for (const TScriptInterface<IColorFilterInterface>& Target : TargetArray->Instances)
        {
            if (Target)
            {
                Target->ColorAction(Color);
            }
        }
    }
}
