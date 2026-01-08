// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SlimeFluidActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

USlimeFluidComponent::USlimeFluidComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("SlimeMesh");
    Mesh->SetupAttachment(this);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetSimulatePhysics(false);
}

void USlimeFluidComponent::BeginPlay()
{
    Super::BeginPlay();

    GenerateSphere();

    /* ˆÀ’è‚µ‚½‹^—‘ÌÏ */
    TargetVolume = Radius * Vertices.Num();
}

void USlimeFluidComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    DetectAllContacts();
    UpdateFluid(DeltaTime);
}

/* ===============================
   Mesh Generation
================================ */

void USlimeFluidComponent::GenerateSphere()
{
    Vertices.Empty();
    Triangles.Empty();

    TArray<FVector> Positions;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;

    for (int32 y = 0; y <= Rings; y++)
    {
        float V = (float)y / Rings;
        float Phi = V * PI;

        for (int32 x = 0; x <= Segments; x++)
        {
            float U = (float)x / Segments;
            float Theta = U * PI * 2;

            FVector P(
                Radius * FMath::Sin(Phi) * FMath::Cos(Theta),
                Radius * FMath::Sin(Phi) * FMath::Sin(Theta),
                Radius * FMath::Cos(Phi)
            );

            FVector N = P.GetSafeNormal();

            FSlimeVertex Vtx;
            Vtx.Position = P;
            Vtx.Velocity = FVector::ZeroVector;
            Vtx.Normal = N;
            Vtx.SurfaceWeight = FMath::Clamp(P.Size() / Radius, 0.f, 1.f);

            Vertices.Add(Vtx);
            Positions.Add(P);
            Normals.Add(N);
            UVs.Add(FVector2D(U, V));
        }
    }

    for (int32 y = 0; y < Rings; y++)
    {
        for (int32 x = 0; x < Segments; x++)
        {
            int32 i0 = x + y * (Segments + 1);
            int32 i1 = i0 + 1;
            int32 i2 = i0 + Segments + 1;
            int32 i3 = i2 + 1;

            Triangles.Append({ i0, i2, i1, i1, i2, i3 });
        }
    }

    Mesh->CreateMeshSection(
        0,
        Positions,
        Triangles,
        Normals,
        UVs,
        {},
        {},
        true
    );
}

/* ===============================
   Contact Detection
================================ */

void USlimeFluidComponent::AddContact(
    const FVector& WorldPos,
    const FVector& WorldNormal,
    float Strength
)
{
    FTransform T = GetComponentTransform();

    FSlimeContact C;
    C.LocalPosition = T.InverseTransformPosition(WorldPos);
    C.Normal = T.InverseTransformVector(WorldNormal).GetSafeNormal();
    C.Strength = Strength;

    Contacts.Add(C);
}

void USlimeFluidComponent::DetectAllContacts()
{
    FVector Center = GetComponentLocation();

    // •¡”•ûŒü‚©‚ç‚ÌÚGŒŸo (‹…ó‚ÉƒŒƒCƒLƒƒƒXƒg)
    TArray<FVector> Directions = {
        FVector(0, 0, -1),  // ‰º
        FVector(1, 0, -0.5), FVector(-1, 0, -0.5),  // Î‚ß‰º
        FVector(0, 1, -0.5), FVector(0, -1, -0.5),
        FVector(1, 0, 0), FVector(-1, 0, 0),  // ‰¡
        FVector(0, 1, 0), FVector(0, -1, 0)
    };

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    for (const FVector& Dir : Directions)
    {
        FVector Start = Center;
        FVector End = Center + Dir.GetSafeNormal() * (Radius + 15.f);

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(
            Hit,
            Start,
            End,
            ECC_WorldStatic,
            Params
        ))
        {
            // ÚG‚Ì‹­“x‚ğ‹——£‚É‰‚¶‚Ä’²®
            float Distance = (Hit.ImpactPoint - Center).Size();
            float Penetration = FMath::Max(0.f, Radius - Distance);
            float ContactStrength = 200.f + Penetration * 50.f;

            AddContact(
                Hit.ImpactPoint,
                Hit.ImpactNormal,
                ContactStrength
            );
        }
    }
}

/* ===============================
   Fluid Simulation
================================ */

void USlimeFluidComponent::UpdateFluid(float DeltaTime)
{
    const FVector Center = FVector::ZeroVector;

    // ‘ÌÏ•Û‘¶‚ÌŒvZ
    float CurrentVolume = 0.f;
    for (const FSlimeVertex& V : Vertices)
    {
        CurrentVolume += V.Position.Size();
    }

    float VolumeError = (TargetVolume - CurrentVolume) / Vertices.Num();

    TArray<FVector> NewPositions;
    NewPositions.Reserve(Vertices.Num());

    for (FSlimeVertex& V : Vertices)
    {
        float Stiffness = FMath::Lerp(CoreStiffness, SurfaceSoftness, V.SurfaceWeight);

        FVector Force = (Center - V.Position) * Stiffness;

        // ‘ÌÏ•Û‘¶‚Ì—Í
        Force += V.Normal * VolumeError * 0.6f;

        // ƒmƒCƒY
        FVector Noise(
            FMath::PerlinNoise1D(GetWorld()->TimeSeconds + V.Position.X),
            FMath::PerlinNoise1D(GetWorld()->TimeSeconds + V.Position.Y),
            FMath::PerlinNoise1D(GetWorld()->TimeSeconds + V.Position.Z)
        );
        Force += Noise * NoiseStrength * V.SurfaceWeight;

        // ÚG‚É‚æ‚é•ÏŒ`
        for (const FSlimeContact& C : Contacts)
        {
            FVector ToV = V.Position - C.LocalPosition;
            float Dist = ToV.Size();

            // ‰e‹¿”ÍˆÍ
            float InfluenceRadius = Radius * ContactInfluenceRadius;

            if (Dist < InfluenceRadius)
            {
                float Falloff = 1.f - (Dist / InfluenceRadius);
                Falloff = FMath::Pow(Falloff, 1.5f); // ‚æ‚è‹}Œƒ‚ÈŒ¸Š

                // –@ü•ûŒü‚Ö‚Ì‰Ÿ‚µ‚İ (‚Â‚Ô‚ê)
                float NormalDot = FVector::DotProduct(ToV.GetSafeNormal(), C.Normal);
                float CompressionForce = C.Strength * Falloff * V.SurfaceWeight;

                if (NormalDot < 0.f) // ÚG–Ê‚ÉŒü‚©‚¤’¸“_
                {
                    Force += -C.Normal * CompressionForce * 1.5f;
                }

                // ÚG–Ê‚É‰ˆ‚Á‚½‰¡•ûŒü‚Ö‚ÌL‚ª‚è (ƒxƒ^ƒb‚ÆL‚ª‚éŒø‰Ê)
                FVector Tangent = ToV - C.Normal * FVector::DotProduct(ToV, C.Normal);
                float TangentLength = Tangent.Size();

                if (TangentLength > 0.01f)
                {
                    Tangent.Normalize();
                    // ÚG“_‚©‚ç‰“‚¢’¸“_‚Ù‚ÇŠO‘¤‚É‰Ÿ‚µo‚·
                    float SpreadForce = CompressionForce * SpreadStrength * (1.f - Falloff);
                    Force += Tangent * SpreadForce;
                }

                // ÚG–Ê‚É’£‚è•t‚­Œø‰Ê (”S’…«)
                if (Dist < Radius * 0.3f)
                {
                    FVector ToContact = C.LocalPosition - V.Position;
                    Force += ToContact * StickinessStrength * Falloff;
                }
            }
        }

        V.Velocity += Force * DeltaTime;
        V.Velocity *= Damping;
        V.Position += V.Velocity * DeltaTime;

        NewPositions.Add(V.Position);
    }

    // ÚG‚ÌŒ¸Š
    for (FSlimeContact& C : Contacts)
    {
        C.Strength *= ContactDecayRate;
    }

    Contacts.RemoveAll(
        [](const FSlimeContact& C)
        {
            return C.Strength < 2.f;
        }
    );

    Mesh->UpdateMeshSection(0, NewPositions, {}, {}, {}, {});
}