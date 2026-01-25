#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h" // これ重要
#include "SlimeComponent.generated.h"

// 物理演算用のパーティクル構造体
USTRUCT()
struct FSlimeParticle
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Position;

    UPROPERTY()
    FVector OldPosition; // 前フレームの位置（Verlet積分用）

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    FVector Force;

    UPROPERTY()
    float Mass;

    FSlimeParticle()
    {
        Position = FVector::ZeroVector;
        OldPosition = FVector::ZeroVector;
        Velocity = FVector::ZeroVector;
        Force = FVector::ZeroVector;
        Mass = 1.0f;
    }
};

// 見た目の頂点を物理粒子に追従させるためのウェイト情報
struct FSlimeSkinningWeight
{
    int32 ParticleIndexA;
    int32 ParticleIndexB;
    float WeightA;
    float WeightB;
    FVector BaseOffsetA;
    FVector BaseOffsetB;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API USlimeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // コンストラクタ
    USlimeComponent();

protected:
    // ゲーム開始時
    virtual void BeginPlay() override;

public:
    // 毎フレーム更新
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // --- パラメータ設定 (ブループリントでいじれるやつ) ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Basic")
    float Radius; // スライムの大きさ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float Stiffness; // 皮の硬さ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float Damping; // 減衰（空気抵抗）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float CoreMass; // 中心の重さ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float FloorFriction; // 床の摩擦

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float ShapeMatchStrength; // 形を保とうとする力

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float PressureStrength; // 膨らむ力（今回は0推奨）

    // --- 解像度設定 ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Resolution")
    int32 VisualRings; // 見た目の分割数（横）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Resolution")
    int32 VisualSegments; // 見た目の分割数（縦）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Resolution")
    int32 PhysicsRings; // 物理の分割数

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Resolution")
    int32 PhysicsSegments; // 物理の分割数

    // --- 内部処理用の変数と関数 ---

protected:
    // Procedural Mesh Componentへの参照
    UPROPERTY()
    UProceduralMeshComponent* ProcMesh;

    // 物理パーティクルの配列
    UPROPERTY()
    TArray<FSlimeParticle> Particles;

    // スキニング情報
    TArray<FSlimeSkinningWeight> SkinningWeights;

    // 見た目メッシュのデータ
    TArray<FVector> VisualVertices;
    TArray<int32> VisualTriangles;
    TArray<FVector> VisualNormals;
    TArray<FVector2D> VisualUVs;
    TArray<FProcMeshTangent> VisualTangents;

    // --- 関数宣言 ---

    // 初期化
    void InitializePhysicsParticles();
    void GenerateVisualMesh();
    void CalculateSkinningWeights();

    // 物理計算メイン
    void Simulate(float DeltaTime);
    void SolveConstraints();      // バネ計算
    void SolveUprightConstraint(); // ★転倒防止（起き上がりこぼし）
    void SolveFloorCollision();   // 床判定

    // 見た目の更新
    void UpdateVisualMeshPosition();

    // プロシージャルメッシュコンポーネントを探すヘルパー
    void FindProceduralMesh();
};