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
    float SurfaceWeight;

    FSlimeVertex()
        : Position(FVector::ZeroVector)
        , Velocity(FVector::ZeroVector)
        , Normal(FVector::UpVector)
        , SurfaceWeight(1.0f)
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

    // Mesh Generation
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void GenerateSphere();

    // Contact
    UFUNCTION(BlueprintCallable, Category = "Slime")
    void AddContact(
        const FVector& WorldPos,
        const FVector& WorldNormal,
        float Strength
    );

protected:
    void DetectAllContacts();
    void UpdateFluid(float DeltaTime);

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

    // Physics Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float CoreStiffness = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float SurfaceSoftness = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float Damping = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Physics")
    float NoiseStrength = 2.0f;

    // Contact Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactInfluenceRadius = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float SpreadStrength = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float StickinessStrength = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime|Contact")
    float ContactDecayRate = 0.92f;

protected:
    UPROPERTY()
    TArray<FSlimeVertex> Vertices;

    UPROPERTY()
    TArray<int32> Triangles;

    UPROPERTY()
    TArray<FSlimeContact> Contacts;

    float TargetVolume;
};
