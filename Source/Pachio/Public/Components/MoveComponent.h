// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/MoveLogic.h"
#include "MoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveComponent();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Init(AActor* owner, TScriptInterface<IMoveLogic>MoveLogic,const float Speed = 1,const FVector NewDirection = FVector(0,0,0));

    UFUNCTION(BlueprintCallable, Category = "Movement")
    FVector Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value = FInputActionValue());

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetDirection(FVector NewDirection);


    //UFUNCTION(BlueprintCallable, Category = "Movement")
    //void SetSpeed(float newSpeed);

public:
    bool SetMoveLogic(TScriptInterface<IMoveLogic>Logic);

private:
    // 現在の移動方向
    FVector CurrentMovementDirection;

    // 速度
    float Speed;

private:
    // コリジョン判定（壁に当たったかどうか）
    //bool IsCollidingWithWall(FVector Direction);
		
    UPROPERTY()
    AActor* mOwner;

private:
    UPROPERTY()
    TScriptInterface<IMoveLogic>MoveLogic;
};
