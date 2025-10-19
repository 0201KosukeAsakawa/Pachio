#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageInfo.generated.h"
// ============================================================================
// ステージ情報関連データ定義
// ステージランクおよびステージ情報のデータテーブル構造体
// ============================================================================

/**
 * @brief ステージクリアランクを定義する列挙体
 *
 * ステージクリア時に取得できるランクを表す。
 * Blueprint でも使用可能。
 */
UENUM(BlueprintType)
enum class EStageRank : uint8
{
    /** 未設定または未クリア状態 */
    None UMETA(DisplayName = "-"),

    /** 標準的なクリアランク */
    B UMETA(DisplayName = "B"),

    /** 高成績クリアランク */
    A UMETA(DisplayName = "A"),

    /** 最高成績クリアランク */
    S UMETA(DisplayName = "S")
};

/**
 * @brief ステージ情報を格納するデータ構造体
 *
 * 各ステージの基本情報（ID、タイトル、難易度、ランク、解放状態など）をまとめた構造体。
 * DataTable 形式で管理され、Blueprint からも参照・編集が可能。
 */
USTRUCT(BlueprintType)
struct FStageInfo : public FTableRowBase
{
    GENERATED_BODY()

    /** ステージを一意に識別するID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString StageID;

    /** ステージタイトル（表示名） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;

    /** ステージ難易度（数値が大きいほど難しい） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Difficulty = 1;

    /** ステージクリア時の達成ランク */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStageRank ClearRank = EStageRank::None;

    /** ステージのジャケット画像（選択画面などで使用） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* JacketImage = nullptr;

    /** ステージが解放済みかどうか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUnlocked = true;

    /**
     * @brief デフォルトコンストラクタ
     *
     * すべてのメンバを既定値で初期化。
     */
    FStageInfo() {}

    /**
     * @brief パラメータ付きコンストラクタ
     *
     * 各種ステージ情報を引数で初期化する。
     *
     * @param InStageID ステージID
     * @param InTitle ステージタイトル
     * @param InDifficulty 難易度
     * @param InClearRank クリアランク
     * @param InJacketImage ジャケット画像
     * @param InUnlocked 解放状態
     */
    FStageInfo(const FString& InStageID, const FString& InTitle, int32 InDifficulty, EStageRank InClearRank, UTexture2D* InJacketImage, bool InUnlocked)
        : StageID(InStageID)
        , Title(InTitle)
        , Difficulty(InDifficulty)
        , ClearRank(InClearRank)
        , JacketImage(InJacketImage)
        , bUnlocked(InUnlocked)
    {
    }
};

