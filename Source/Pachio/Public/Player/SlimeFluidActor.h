// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"
#include "SlimeFluidActor.generated.h"

/**
 * スライムを構成する単一頂点データ
 * - 物理挙動
 * - 表面／コア判定
 * - 法線管理
 */
USTRUCT(BlueprintType)
struct FSlimeVertex
{
    GENERATED_BODY()

    /** 現在の頂点位置（ローカル座標） */
    UPROPERTY()
    FVector Position;

    /** 頂点の速度（物理計算用） */
    UPROPERTY()
    FVector Velocity;

    /** 頂点の法線（描画・表面方向判定用） */
    UPROPERTY()
    FVector Normal;

    /** 表面重み（0=完全コア, 1=完全表面） */
    UPROPERTY()
    float SurfaceWeight;

    /** コア頂点かどうかのフラグ */
    UPROPERTY()
    bool bIsCore;

    FSlimeVertex()
        : Position(FVector::ZeroVector)
        , Velocity(FVector::ZeroVector)
        , Normal(FVector::UpVector)
        , SurfaceWeight(1.0f)
        , bIsCore(false)
    {
    }
};

/**
 * スライムへの接触情報
 * - 外力
 * - 押し込み
 * - 地面判定
 */
USTRUCT(BlueprintType)
struct FSlimeContact
{
    GENERATED_BODY()

    /** 接触点のローカル座標 */
    UPROPERTY()
    FVector LocalPosition;

    /** 接触面の法線 */
    UPROPERTY()
    FVector Normal;

    /** 接触の強さ */
    UPROPERTY()
    float Strength;

    /** 地面接触かどうか */
    UPROPERTY()
    bool bIsGround;

    FSlimeContact()
        : LocalPosition(FVector::ZeroVector)
        , Normal(FVector::UpVector)
        , Strength(0.0f)
        , bIsGround(false)
    {
    }
};

/**
 * 頂点の隣接情報
 * - 力の伝播計算用
 */
USTRUCT(BlueprintType)
struct FVertexNeighborData
{
    GENERATED_BODY()

    /** 隣接頂点のインデックス配列 */
    UPROPERTY()
    TArray<int32> Neighbors;

    FVertexNeighborData()
    {
    }
};

/*
 * スライムの流体挙動・変形・接触処理を管理するコンポーネント
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API USlimeFluidComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    USlimeFluidComponent();

protected:
    /*
     * コンポーネント初期化処理
     *
     * @return void
     */
    virtual void BeginPlay() override;

public:
    /*
     * 毎フレーム更新処理
     *
     * @param DeltaTime フレーム時間
     * @param TickType Tick種別
     * @param ThisTickFunction Tick関数
     *
     * @return void
     */
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    /*
     * 初期球形メッシュを生成する
     *
     * @return void
     */
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void GenerateSphere();

    /*
     * 外部から接触情報を追加する
     *
     * @param WorldPos 接触位置（ワールド座標）
     * @param WorldNormal 接触法線
     * @param Strength 接触強度
     *
     * @return void
     */
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void AddContact(
        const FVector& WorldPos,
        const FVector& WorldNormal,
        float Strength
    );

protected:
    /*
     * 全頂点に対する接触検出
     *
     * @return void
     */
    void DetectAllContacts();

    /*
     * スライムの物理挙動更新
     *
     * @param DeltaTime フレーム時間
     *
     * @return void
     */
    void UpdateFluid(float DeltaTime);

    /*
     * デバッグ可視化描画
     *
     * @return void
     */
    void DrawDebugVisualization();

    /*
     * コア中心位置の更新
     *
     * @param DeltaTime フレーム時間
     *
     * @return void
     */
    void UpdateCoreCenter(float DeltaTime);

    /*
     * 力を隣接頂点へ伝播させる
     *
     * @param Forces 頂点ごとの力配列
     * @param DeltaTime フレーム時間
     *
     * @return void
     */
    void PropagateForces(TArray<FVector>& Forces, float DeltaTime);

    /*
     * 頂点の隣接情報を構築する
     *
     * @return void
     */
    void BuildVertexNeighbors();

public:
    /** スライム描画用プロシージャルメッシュ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slime")
    UProceduralMeshComponent* Mesh;

    /** スライム半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    float Radius = 50.0f;

    /** 経度方向分割数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Segments = 16;

    /** 緯度方向分割数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Rings = 12;

    /** コア半径比率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CoreRadiusRatio = 0.5f;

    /** コアの剛性 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreStiffness = 15.0f;

    /** コアの重心追従強度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreFollowStrength = 6.0f;

    /** 変形時にコア中心を移動させるか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bAllowCenterMovement = true;

    /** 復元時にコア中心を戻すか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bResetCenterToOrigin = true;

    /** コア中心の復元速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    float CenterResetSpeed = 2.0f;

    /** ワールド基準で中心を戻すか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    bool bUseWorldOrigin = false;

    /** 表面の剛性 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceStiffness = 8.0f;

    /** 表面の柔らかさ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceSoftness = 2.0f;

    /** 速度減衰率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float Damping = 0.85f;

    /** 追従ラグ強度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float FollowLagStrength = 12.0f;

    /** ノイズ強度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float NoiseStrength = 2.0f;

    /** ぷるぷる揺れ量 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float JiggleAmount = 5.0f;

    /** 弾性倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float BounceFactor = 1.2f;

    /** 変形速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float DeformationSpeed = 1.0f;

    /** 復元速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float RecoverySpeed = 1.0f;

    /** 力の伝播を有効にするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    bool bEnableForcePropagate = true;

    /** 伝播強度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float PropagationStrength = 5.0f;

    /** 伝播減衰率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float PropagationDamping = 0.7f;

    /** 伝播反復回数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    int32 PropagationIterations = 2;

    /** 接触影響半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float ContactInfluenceRadius;

    /** コアに加わる最大力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float MaxCoreForce = 500.0f;

    /** 頂点ごとの最大力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float MaxForcePerVertex = 1000.0f;

protected:
    /** スライム頂点配列 */
    UPROPERTY()
    TArray<FSlimeVertex> Vertices;

    /** 三角形インデックス */
    UPROPERTY()
    TArray<int32> Triangles;

    /** 接触情報配列 */
    UPROPERTY()
    TArray<FSlimeContact> Contacts;

    /** 目標平均半径 */
    float TargetAverageRadius;

    /** 経過時間 */
    float ElapsedTime = 0.f;

    /** 動的コア中心 */
    UPROPERTY()
    FVector LocalCoreCenter;

    /** 初期頂点位置（デバッグ用） */
    UPROPERTY()
    TArray<FVector> InitialVertexPositions;

    /** 頂点隣接情報 */
    UPROPERTY()
    TArray<FVertexNeighborData> VertexNeighbors;
};