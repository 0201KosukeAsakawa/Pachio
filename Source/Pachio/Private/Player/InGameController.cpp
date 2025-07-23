// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InGameController.h"
#include "Objects/ControllableObjectBase.h"

// 入力をセットアップする関数
void AInGameController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // "TogglePossess" アクション（キーボードなど）に TogglePossession 関数をバインド
    InputComponent->BindAction("TogglePossess", IE_Pressed, this, &AInGameController::TogglePossession);
}

// 所有権を切り替える関数
void AInGameController::TogglePossession()
{
    // すでに何かを憑依していて、元のPawnが記録されている場合は元に戻る
    if (bIsPossessing && OriginalPawn)
    {
        Possess(OriginalPawn);        // 元のPawnに所有権を戻す
        bIsPossessing = false;        // 所有状態フラグをリセット
        return;
    }

    // 憑依可能な対象を探す
    AActor* HitActor = FindPossessableObject();
    if (AControllableObjectBase* Target = Cast<AControllableObjectBase>(HitActor))
    {
        OriginalPawn = GetPawn();                     // 現在のPawnを保存
        Possess(Cast<APawn>(Target));                // 対象をPawnとしてキャストして憑依
        bIsPossessing = true;                         // 所有状態フラグをセット
    }
}

// 手動で元のPawnに戻る処理（外部から呼ばれる用）
void AInGameController::ReturnToOriginalPlayer()
{
    if (OriginalPawn)
    {
        Possess(OriginalPawn); // 元のPawnに所有権を戻す
    }
}

AActor* AInGameController::FindPossessableObject()
{
    APawn* PlayerPawn = GetPawn();
    if (!PlayerPawn) return nullptr;

    FVector Start = PlayerPawn->GetActorLocation();        // ★ プレイヤー本体の位置
    FRotator Rotation = PlayerPawn->GetActorRotation();    // ★ プレイヤー本体の向き（前方）
    FVector End = Start + Rotation.Vector() * DetectionDistance;

    const FQuat TraceRotation = FQuat::Identity;
    const FVector EnlargedBoxHalfSize = BoxHalfSize;
    const FCollisionShape Box = FCollisionShape::MakeBox(EnlargedBoxHalfSize);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlayerPawn);

    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        TraceRotation,
        ECC_Visibility,
        Box,
        Params
    );
#if WITH_EDITOR
    // デバッグ描画：Boxスイープの始点
    DrawDebugBox(
        GetWorld(),
        Start,
        EnlargedBoxHalfSize,
        FQuat::Identity,
        FColor::Green,
        false, // bPersistentLines
        2.0f,  // LifeTime（秒）
        0,     // DepthPriority
        1.0f   // Thickness
    );

    // デバッグ描画：Boxスイープの終点
    DrawDebugBox(
        GetWorld(),
        End,
        EnlargedBoxHalfSize,
        FQuat::Identity,
        FColor::Red,
        false,
        2.0f,
        0,
        1.0f
    );
#endif

    if (bHit)
    {
        AActor* ClosestActor = nullptr;
        float ClosestDistanceSq = TNumericLimits<float>::Max();

        for (const FHitResult& Hit : HitResults)
        {
            AActor* TargetActor = Hit.GetActor();
            if (TargetActor && TargetActor->ActorHasTag(TEXT("Possessable")))
            {
                const float DistanceSq = FVector::DistSquared(Start, TargetActor->GetActorLocation());
                if (DistanceSq < ClosestDistanceSq)
                {
                    ClosestDistanceSq = DistanceSq;
                    ClosestActor = TargetActor;
                }
            }
        }

        return ClosestActor;
    }

    return nullptr;
}
