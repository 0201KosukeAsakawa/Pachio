// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"
#include "SlimeFluidActor.generated.h"

/**
* スライムの頂点データ構造体
* 各頂点の位置、速度、法線、表面重み、コアフラグを保持
*
}*/
USTRUCT(BlueprintType)
struct FSlimeVertex
{
    GENERATED_BODY()

    /** 頂点のローカル座標位置 */
    UPROPERTY()
    FVector Position;

    /** 頂点の速度ベクトル */
    UPROPERTY()
    FVector Velocity;

    /** 頂点の法線ベクトル */
    UPROPERTY()
    FVector Normal;

    /** 表面重み (0=コア頂点, 1=表面頂点) */
    UPROPERTY()
    float SurfaceWeight;

    /** コア頂点であるかどうかのフラグ */
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
 * スライムの接触情報構造体
 * 外部オブジェクトとの接触点の位置、法線、強度を保持
 */
USTRUCT(BlueprintType)
struct FSlimeContact
{
    GENERATED_BODY()

    /** 接触点のローカル座標位置 */
    UPROPERTY()
    FVector LocalPosition;

    /** 接触面の法線ベクトル */
    UPROPERTY()
    FVector Normal;

    /** 接触の強度 (0.0～1.0以上) */
    UPROPERTY()
    float Strength;

    /** 地面接触かどうかのフラグ */
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
 * 頂点の隣接情報構造体
 * 各頂点に隣接する頂点のインデックスリストを保持
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

/**
 * スライム流体シミュレーションコンポーネント
 * 柔軟な球体メッシュを生成し、物理シミュレーションにより変形・復元を行う
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API USlimeFluidComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    USlimeFluidComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    /**
     * 球体メッシュを生成
     * Radius, Segments, Rings パラメータに基づいてスライムメッシュを作成
     */
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void GenerateSphere();

    /**
     * 接触点を手動で追加
     * @param WorldPos ワールド座標での接触位置
     * @param WorldNormal ワールド座標での接触法線
     * @param Strength 接触の強度 (0.0～1.0以上)
     */
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void AddContact(
        const FVector& WorldPos,
        const FVector& WorldNormal,
        float Strength
    );

protected:
    /**
     * 全頂点に対して接触検出を実行
     * メッシュ形状に基づいた衝突チェックを行い、接触情報を更新
     */
    void DetectAllContacts();

    /**
     * 流体シミュレーションの更新処理
     * @param DeltaTime フレーム間の経過時間
     */
    void UpdateFluid(float DeltaTime);

    /**
     * デバッグビジュアライゼーションの描画
     * コア頂点、表面頂点、接触点などを可視化
     */
    void DrawDebugVisualization();

    /**
     * コア中心位置の更新処理
     * @param DeltaTime フレーム間の経過時間
     */
    void UpdateCoreCenter(float DeltaTime);

    /**
     * 接触点から周辺頂点への力の伝播処理
     * @param Forces 各頂点に適用される力の配列
     * @param DeltaTime フレーム間の経過時間
     */
    void PropagateForces(TArray<FVector>& Forces, float DeltaTime);

    /**
     * 頂点の隣接関係を構築
     * トライアングルメッシュから各頂点の隣接頂点を計算
     */
    void BuildVertexNeighbors();

public:
    // Components
    /** プロシージャルメッシュコンポーネント（スライムの形状を描画） */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slime")
    UProceduralMeshComponent* Mesh;

    // Mesh Parameters
    /** スライムの半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    float Radius = 50.0f;

    /** 水平方向の分割数（多いほど滑らか） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Segments = 16;

    /** 垂直方向の分割数（多いほど滑らか） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Rings = 12;

    // Core Parameters (コア保護)
    /** コアの半径比率 (0.5 = 半径の50%がコア) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CoreRadiusRatio = 0.5f;

    /** コア頂点の剛性（高いほど変形しにくい） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreStiffness = 15.0f;

    /** コアが重心に追従する強度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreFollowStrength = 6.0f;

    /** 変形時にセンター位置を移動させるか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bAllowCenterMovement = true;

    /** 変形から戻る際にセンターを原点に戻すか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bResetCenterToOrigin = true;

    /** センターが原点に戻る速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    float CenterResetSpeed = 2.0f;

    /** true=ワールド座標でオーナー位置に戻す, false=ローカル(0,0,0)に戻す */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    bool bUseWorldOrigin = false;

    // Physics Parameters
    /** 表面頂点の剛性（高いほど硬い） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceStiffness = 8.0f;

    /** 表面の柔らかさ（高いほど柔らかく変形する） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceSoftness = 2.0f;

    /** 速度の減衰率 (0.0～1.0, 1.0に近いほど減衰が少ない) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float Damping = 0.85f;

    /** 原点への追従強度（ラグの強さ） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float FollowLagStrength = 12.0f;

    /** ノイズによる揺らぎの強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float NoiseStrength = 2.0f;

    /** ぷるぷる揺れの強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float JiggleAmount = 5.0f;

    /** 弾む感じの強さ（オーバーシュート） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float BounceFactor = 1.2f;

    /** 変形速度の倍率 (1.0 = 通常, 2.0 = 2倍速, 0.5 = 半分) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float DeformationSpeed = 1.0f;

    /** 復元速度（元の形に戻る速さ） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float RecoverySpeed = 1.0f;

    /** 力の伝播を有効にするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    bool bEnableForcePropagate = true;

    /** 力の伝播の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float PropagationStrength = 5.0f;

    /** 伝播時の減衰率 (0.7 = 30%減衰) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float PropagationDamping = 0.7f;

    /** 伝播の反復回数 (1=隣接のみ、2=隣接の隣接まで) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    int32 PropagationIterations = 2;

    /** 接触の影響範囲半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float ContactInfluenceRadius;

    /** コアに適用できる最大力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float MaxCoreForce = 500.0f;

    /** 1頂点あたりの最大力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float MaxForcePerVertex = 1000.0f;

    // Volume Preservation (体積保存)
    /** 体積保存を有効にするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume")
    bool bPreserveVolume = true;

    /** 体積保存の剛性（高いほど体積を維持） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreserveVolume"))
    float VolumeStiffness = 20.0f;

    /** 表面張力（高いほど球形を維持） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreserveVolume"))
    float SurfaceTension = 8.0f;

    /** めり込み防止を有効にするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume")
    bool bPreventPenetration = true;

    /** めり込み抵抗力の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreventPenetration"))
    float PenetrationResistance = 50.0f;

    /** 接触の広がり強度（周辺への影響範囲） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float SpreadStrength = 0.8f;

    /** 接触情報の減衰率（時間経過での消失速度） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDecayRate = 0.92f;

    /** 接触の方向性 (0=全方向, 1=接触方向のみ) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDirectionality = 0.8f;

    /** 反対側の変形を防ぐか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    bool bPreventOppositeSideDeformation = true;

    /** メッシュ形状に基づいた接触検出を使用するか（推奨） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    bool bUseMeshBasedCollision = true;

    /** 何頂点ごとに接触チェックするか (1=全頂点、3=3頂点ごと) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact", meta = (EditCondition = "bUseMeshBasedCollision"))
    int32 CollisionCheckVertexStep = 3;

    /** 接触チェックのレイキャスト距離 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact", meta = (EditCondition = "bUseMeshBasedCollision"))
    float CollisionCheckDistance = 15.0f;

    /** 接触点を表面からこの距離だけ外側に配置（めり込み防止） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactPenetrationOffset = 5.0f;

    // Ground Contact (地面接触専用)
    /** 地面接触を特別に処理するか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground")
    bool bEnableGroundSpecialHandling = false;

    /** 地面接触時の潰れ倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    float GroundSquashMultiplier = 1.5f;

    /** 地面への粘着力 (0=なし, 1=強い) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    float GroundStickiness = 0.3f;

    /** この角度以下を地面として扱う（度数、上向きベクトルとの角度） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    float GroundAngleThreshold = 45.0f;

    /** 地面接触時に底面を平らにするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    bool bFlattenBottomOnGround = true;

    /** 底面平面化の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bFlattenBottomOnGround"))
    float BottomFlattenStrength = 10.0f;

    /** 底面と判定する範囲 (0.5=下半分, 0.7=下70%) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bFlattenBottomOnGround"))
    float BottomFlattenRadius = 0.7f;

    // Material
    /** スライムに適用するマテリアル */
    UPROPERTY(EditAnywhere, Category = "Slime|Material")
    UMaterialInterface* SlimeMaterial;

    // Debug
    /** 頂点のデバッグ表示を有効にするか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowVertexDebug = false;

    /** コア頂点を表示するか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowCoreVertices = true;

    /** 表面頂点を表示するか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowSurfaceVertices = true;

    /** コア中心点を表示するか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowCoreCenter = true;

    /** 接触点を表示するか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowContactPoints = true;

    /** 実際のメッシュ形状を線で表示するか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowActualMeshShape = true;

    /** 初期形状（球）を表示するか */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowInitialShape = false;

    /** false=ローカル座標, true=ワールド座標で表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowWorldCoordinates = false;

    /** デバッグポイントのサイズ */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    float DebugPointSize = 5.0f;

protected:
    /** 全頂点データの配列 */
    UPROPERTY()
    TArray<FSlimeVertex> Vertices;

    /** メッシュのトライアングルインデックス配列 */
    UPROPERTY()
    TArray<int32> Triangles;

    /** 現在アクティブな接触情報の配列 */
    UPROPERTY()
    TArray<FSlimeContact> Contacts;

    /** 目標とする平均半径 */
    float TargetAverageRadius;

    /** 経過時間（ノイズ計算用） */
    float ElapsedTime = 0.f;

    /** 動的に変化するコア中心のローカル座標 */
    UPROPERTY()
    FVector LocalCoreCenter;

    /** 初期形状の頂点位置配列（デバッグ表示用） */
    UPROPERTY()
    TArray<FVector> InitialVertexPositions;

    /** 頂点の隣接情報配列（力の伝播用） */
    UPROPERTY()
    TArray<FVertexNeighborData> VertexNeighbors;
};