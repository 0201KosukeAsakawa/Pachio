#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageInfo.generated.h"

UENUM(BlueprintType)
enum class EStageRank : uint8
{
    None UMETA(DisplayName = "-"),
    B UMETA(DisplayName = "B"),
    A UMETA(DisplayName = "A"),
    S UMETA(DisplayName = "S")
};

USTRUCT(BlueprintType)
struct FStageInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString StageID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Difficulty = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStageRank ClearRank = EStageRank::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* JacketImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUnlocked = true;

    // コンストラクタを追加（初期化リスト対応）
    FStageInfo() {}

    FStageInfo(const FString& InStageID, const FString& InTitle, int32 InDifficulty, EStageRank InClearRank, UTexture2D* InJacketImage, bool InUnlocked)
        : StageID(InStageID), Title(InTitle), Difficulty(InDifficulty), ClearRank(InClearRank), JacketImage(InJacketImage), bUnlocked(InUnlocked)
    {
    }
};
