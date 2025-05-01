// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveComponent();

    void Init(AActor*);

    // 移動処理を行う関数
    virtual void Movement(float DeltaTime);

    // 生成時にDirectionを設定
    void SetDirection(FVector NewDirection);

private:
    // 現在の移動方向
    FVector CurrentMovementDirection;

    // 速度
    float Speed;

private:
    // コリジョン判定（壁に当たったかどうか）
    bool IsCollidingWithWall(FVector Direction);
		
    UPROPERTY()
    AActor* mOwner;
};
