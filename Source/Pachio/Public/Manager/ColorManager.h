#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "ColorManager.generated.h"

class IColorFilterInterface;

// �F���[�h�񋓑�
UENUM(BlueprintType)
enum class EColorMode : uint8
{
    Layer      UMETA(DisplayName = "Layer"),
    Object     UMETA(DisplayName = "Object"),
    Background UMETA(DisplayName = "Background")
};

// �u���[�v�����g�N���X��ێ�����\���́i�G�f�B�^�ݒ�p�j
USTRUCT(BlueprintType)
struct FColorTargetArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<UObject>> Targets;
};

// ���s���C���X�^���X��ێ�����\����
USTRUCT()
struct FColorTargetInstanceArray
{
    GENERATED_BODY()

    TArray<TScriptInterface<IColorFilterInterface>> Instances;
};

// UColorManager�N���X�{��
UCLASS(Blueprintable)
class UColorManager : public UObject
{
    GENERATED_BODY()

private:
    // �G�f�B�^�Őݒ肷��u���[�v�����g�N���X�Q
    UPROPERTY(EditAnywhere)
    TMap<EColorMode, FColorTargetArray> ColorTargetsClass;

    // ���s���ɐ������ꂽ�C���X�^���X�Q
    UPROPERTY()
    TMap<EColorMode, FColorTargetInstanceArray> ColorTargets;

    UPROPERTY()
    TScriptInterface<IColorFilterInterface> ActiveLayerTarget;

    // ���݂̃��[�h
    UPROPERTY(EditAnywhere)
    EColorMode Mode;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* PostProcessMaterial;

    UPROPERTY()
    UMaterialInstanceDynamic* PostProcessMID;

public:
    // �������֐��i�u���[�v�����g�N���X����C���X�^���X�𐶐��j
    void InitializeTargets();

    // �F��K�p����֐�
    UFUNCTION()
    void ApplyColor(FLinearColor NewColor);

    // �^�[�Q�b�g��ǉ��o�^����֐��i�K�v�ɉ����āj
    void RegisterTarget(EColorMode Mode, TScriptInterface<IColorFilterInterface> Target);
};
