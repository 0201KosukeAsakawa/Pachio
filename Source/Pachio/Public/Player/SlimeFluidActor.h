// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"
#include "SlimeFluidActor.generated.h"


/**
 * スライムを構成する1頂点の物理データ
 * - 表面 / コアの区別
 * - 物理挙動（速度・法線）
 */
USTRUCT(BlueprintType)
struct FSlimeVertex
{
    GENERATED_BODY()

    /** 現在の頂点位置（ローカル座標） */
    UPROPERTY()
    FVector Position;

    /** 頂点に加算される速度（物理シミュレーション用） */
    UPROPERTY()
    FVector Velocity;

    /** 表面法線（主に表面張力・押し戻し計算に使用） */
    UPROPERTY()
    FVector Normal;

    /**
     * 表面ウェイト
     * 0.0 = 完全なコア
     * 1.0 = 完全な表面
     */
    UPROPERTY()
    float SurfaceWeight;

    /** true の場合、この頂点はコアに属する */
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
 * スライムと外部オブジェクトの接触情報
 * - 接触位置
 * - 接触方向
 * - 影響の強さ
 */
USTRUCT(BlueprintType)
struct FSlimeContact
{
    GENERATED_BODY()

    /** 接触位置（スライムローカル座標） */
    UPROPERTY()
    FVector LocalPosition;

    /** 接触面の法線 */
    UPROPERTY()
    FVector Normal;

    /** 接触の強さ（押し込み量・衝撃量など） */
    UPROPERTY()
    float Strength;

    /** true の場合、地面として扱う接触 */
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
 * - 力の伝播
 * - 変形の拡散
 */
USTRUCT(BlueprintType)
struct FVertexNeighborData
{
    GENERATED_BODY()

    /** 隣接している頂点インデックス配列 */
    UPROPERTY()
    TArray<int32> Neighbors;

    FVertexNeighborData()
    {
    }
};


 /**
  * スライムの流体挙動を制御するコンポーネント
  * ・プロシージャルメッシュ生成
  * ・頂点ベース疑似流体シミュレーション
  * ・接触・体積保持・地面処理
  */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API USlimeFluidComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    /**
     * コンストラクタ
     */
    USlimeFluidComponent();

protected:
    /**
     * ゲーム開始時の初期化処理
     */
    virtual void BeginPlay() override;

public:
    /**
     * 毎フレーム呼ばれる更新処理
     *
     * @param DeltaTime フレーム間の経過時間
     * @param TickType Tickの種類
     * @param ThisTickFunction Tick関数情報
     * @return なし
     */
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    /**
     * スライムの初期形状（球体）を生成する
     *
     * @param なし
     * @return なし
     */
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void GenerateSphere();

    /**
     * 外部から接触情報を追加する
     *
     * @param WorldPos 接触位置（ワールド座標）
     * @param WorldNormal 接触面法線（ワールド）
     * @param Strength 接触の強さ
     * @return なし
     */
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void AddContact(
        const FVector& WorldPos,
        const FVector& WorldNormal,
        float Strength
    );

protected:
    /**
     * 全頂点に対して接触判定を行う
     *
     * @param なし
     * @return なし
     */
    void DetectAllContacts();

    /**
     * 流体シミュレーションの更新処理
     *
     * @param DeltaTime 経過時間
     * @return なし
     */
    void UpdateFluid(float DeltaTime);

    /**
     * デバッグ描画を行う
     *
     * @param なし
     * @return なし
     */
    void DrawDebugVisualization();

    /**
     * コア中心位置を更新する
     *
     * @param DeltaTime 経過時間
     * @return なし
     */
    void UpdateCoreCenter(float DeltaTime);

    /**
     * 力を隣接頂点へ伝播させる
     *
     * @param Forces 各頂点にかかる力
     * @param DeltaTime 経過時間
     * @return なし
     */
    void PropagateForces(TArray<FVector>& Forces, float DeltaTime);

    /**
     * 頂点の隣接関係を構築する
     *
     * @param なし
     * @return なし
     */
    void BuildVertexNeighbors();

public:
    /** プロシージャルメッシュ本体 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slime")
    UProceduralMeshComponent* Mesh;

    /** スライムの基本半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    float Radius = 50.0f;

    /** メッシュの横分割数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Segments = 16;

    /** メッシュの縦分割数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Rings = 12;

    /** コア領域の半径比率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CoreRadiusRatio = 0.5f;

    /** コアの変形しにくさ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreStiffness = 15.0f;

    /** コアが重心へ追従する強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreFollowStrength = 6.0f;

    /** 変形時にコア中心を動かすか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bAllowCenterMovement = true;

    /** 復元時に中心を元に戻すか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bResetCenterToOrigin = true;

    /** 中心が戻る速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    float CenterResetSpeed = 2.0f;

    /** ワールド座標で原点復帰するか */
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

    /** 追従遅延の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float FollowLagStrength = 12.0f;

    /** ノイズ変形量 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float NoiseStrength = 2.0f;

    /** 揺れの強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float JiggleAmount = 5.0f;

    /** バウンド強度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float BounceFactor = 1.2f;

    /** 変形速度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float DeformationSpeed = 1.0f;

    /** 復元速度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float RecoverySpeed = 1.0f;

    /** 力の伝播を有効にするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    bool bEnableForcePropagate = true;

    /** 伝播の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float PropagationStrength = 5.0f;

    /** 伝播時の減衰率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    float PropagationDamping = 0.7f;

    /** 伝播反復回数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics", meta = (EditCondition = "bEnableForcePropagate"))
    int32 PropagationIterations = 2;

    /** 体積保存を行うか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume")
    bool bPreserveVolume = true;

    /** 体積保持の剛性 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreserveVolume"))
    float VolumeStiffness = 20.0f;

    /** 表面張力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreserveVolume"))
    float SurfaceTension = 8.0f;

    /** めり込み防止を行うか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume")
    bool bPreventPenetration = true;

    /** めり込み抵抗力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreventPenetration"))
    float PenetrationResistance = 50.0f;

    /** 接触の拡散率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float SpreadStrength = 0.8f;

    /** 接触影響の減衰率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDecayRate = 0.92f;

    /** 接触方向性の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDirectionality = 0.8f;

    /** 反対側変形を防ぐか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    bool bPreventOppositeSideDeformation = true;

    /** メッシュベース衝突判定を使うか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    bool bUseMeshBasedCollision = true;

    /** 接触チェック間隔 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact", meta = (EditCondition = "bUseMeshBasedCollision"))
    int32 CollisionCheckVertexStep = 3;

    /** 接触判定距離 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact", meta = (EditCondition = "bUseMeshBasedCollision"))
    float CollisionCheckDistance = 15.0f;

    /** 接触位置オフセット */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactPenetrationOffset = 5.0f;

    /** 地面特別処理を有効にするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground")
    bool bEnableGroundSpecialHandling = false;

    /** 地面潰れ倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    float GroundSquashMultiplier = 1.5f;

    /** 地面粘着力 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    float GroundStickiness = 0.3f;

    /** 地面と判定する角度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    float GroundAngleThreshold = 45.0f;

    /** 底面を平らにするか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bEnableGroundSpecialHandling"))
    bool bFlattenBottomOnGround = true;

    /** 平面化の強さ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bFlattenBottomOnGround"))
    float BottomFlattenStrength = 10.0f;

    /** 平面判定範囲 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Ground", meta = (EditCondition = "bFlattenBottomOnGround"))
    float BottomFlattenRadius = 0.7f;

    /** スライム用マテリアル */
    UPROPERTY(EditAnywhere, Category = "Slime|Material")
    UMaterialInterface* SlimeMaterial;

    /** 頂点デバッグ表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowVertexDebug = false;

    /** コア頂点表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowCoreVertices = true;

    /** 表面頂点表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowSurfaceVertices = true;

    /** コア中心表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowCoreCenter = true;

    /** 接触点表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowContactPoints = true;

    /** 実メッシュ形状表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowActualMeshShape = true;

    /** 初期形状表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowInitialShape = false;

    /** ワールド座標表示 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowWorldCoordinates = false;

    /** デバッグ点サイズ */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    float DebugPointSize = 5.0f;

    /** 接触影響範囲 */
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    float ContactInfluenceRadius;

protected:
    /** スライム頂点配列 */
    UPROPERTY()
    TArray<FSlimeVertex> Vertices;

    /** メッシュ三角形インデックス */
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
