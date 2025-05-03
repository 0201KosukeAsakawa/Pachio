//-------------------------------------------
// ファイル名	：EnemyCharactorBasic.h
// 内容			：敵キャラクターに共通する処理全般（移動・回転・接触）
//-------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyCharactorBasic.generated.h"

class UMoveComponent;
class UPhysicsCalculator;

UCLASS()
class PACHIO_API AEnemyCharactorBasic : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharactorBasic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComponent;

private:
	UPROPERTY();
	UMoveComponent* MoveComp;

	UPROPERTY();
	UPhysicsCalculator* PhysicsCal;

};
