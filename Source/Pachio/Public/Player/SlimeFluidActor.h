// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"
#include "SlimeFluidActor.generated.h"

USTRUCT(BlueprintType)
struct FSlimeVertex
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Position;

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    FVector Normal;

    UPROPERTY()
    float SurfaceWeight; // 0=コア, 1=表面

    UPROPERTY()
    bool bIsCore; // コア頂点フラグ

    FSlimeVertex()
        : Position(FVector::ZeroVector)
        , Velocity(FVector::ZeroVector)
        , Normal(FVector::UpVector)
        , SurfaceWeight(1.0f)
        , bIsCore(false)
    {
    }
};

USTRUCT(BlueprintType)
struct FSlimeContact
{
    GENERATED_BODY()

    UPROPERTY()
    FVector LocalPosition;

    UPROPERTY()
    FVector Normal;

    UPROPERTY()
    float Strength;

    FSlimeContact()
        : LocalPosition(FVector::ZeroVector)
        , Normal(FVector::UpVector)
        , Strength(0.0f)
    {
    }
};

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

    UFUNCTION(BlueprintCallable, Category = "Slime")
    void GenerateSphere();

    UFUNCTION(BlueprintCallable, Category = "Slime")
    void AddContact(
        const FVector& WorldPos,
        const FVector& WorldNormal,
        float Strength
    );

protected:
    void DetectAllContacts();
    void UpdateFluid(float DeltaTime);
    void DrawDebugVisualization();
    void UpdateCoreCenter(float DeltaTime);

public:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slime")
    UProceduralMeshComponent* Mesh;

    // Mesh Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    float Radius = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Segments = 16;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Mesh")
    int32 Rings = 12;

    // Core Parameters (コア保護)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CoreRadiusRatio = 0.5f; // コアの半径比率（0.5 = 半径の50%がコア）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreStiffness = 15.0f; // コアの剛性

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    float CoreFollowStrength = 6.0f; // コアが重心に追従する強度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bAllowCenterMovement = true; // 変形時にセンターを移動させるか

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core")
    bool bResetCenterToOrigin = true; // 変形から戻る際にセンターを原点に戻すか

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    float CenterResetSpeed = 2.0f; // センターが原点に戻る速度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Core", meta = (EditCondition = "bResetCenterToOrigin"))
    bool bUseWorldOrigin = false; // true=ワールド座標でオーナー位置に戻す, false=ローカル(0,0,0)に戻す

    // Physics Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceStiffness = 8.0f; // 表面の剛性

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceSoftness = 2.0f; // 表面の柔らかさ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float Damping = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float FollowLagStrength = 12.0f; // 追従ラグ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float NoiseStrength = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float DeformationSpeed = 1.0f; // 変形速度（1.0 = 通常, 2.0 = 2倍速, 0.5 = 半分）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float RecoverySpeed = 1.0f; // 復元速度（元の形に戻る速さ）

    // Volume Preservation (体積保存)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume")
    bool bPreserveVolume = true; // 体積保存を有効にするか

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreserveVolume"))
    float VolumeStiffness = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreserveVolume"))
    float SurfaceTension = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume")
    bool bPreventPenetration = true; // めり込み防止

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Volume", meta = (EditCondition = "bPreventPenetration"))
    float PenetrationResistance = 50.0f; // めり込み抵抗力

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float SpreadStrength = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDecayRate = 0.92f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDirectionality = 0.8f; // 接触の方向性（0=全方向, 1=接触方向のみ）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    bool bPreventOppositeSideDeformation = true; // 反対側の変形を防ぐ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    bool bUseMeshBasedCollision = true; // メッシュ形状に基づいた接触検出

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactPenetrationOffset = 5.0f; // 接触点を表面からこの距離だけ外側に配置

    // Material
    UPROPERTY(EditAnywhere, Category = "Slime|Material")
    UMaterialInterface* SlimeMaterial;

    // Debug
    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowVertexDebug = false;

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowCoreVertices = true;

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowSurfaceVertices = true;

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowCoreCenter = true;

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowContactPoints = true;

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowActualMeshShape = true; // 実際のメッシュ形状を線で表示

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowInitialShape = false; // 初期形状（球）を表示

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    bool bShowWorldCoordinates = false; // false=ローカル座標, true=ワールド座標

    UPROPERTY(EditAnywhere, Category = "Slime|Debug")
    float DebugPointSize = 5.0f;

        // Contact Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactInfluenceRadius = 1.2f;
protected:
    UPROPERTY()
    TArray<FSlimeVertex> Vertices;

    UPROPERTY()
    TArray<int32> Triangles;

    UPROPERTY()
    TArray<FSlimeContact> Contacts;

    float TargetAverageRadius;
    float ElapsedTime = 0.f;

    UPROPERTY()
    FVector LocalCoreCenter; // 動的コア中心

    // 初期形状の保存（デバッグ用）
    UPROPERTY()
    TArray<FVector> InitialVertexPositions;
};