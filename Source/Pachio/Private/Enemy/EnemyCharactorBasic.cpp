//-------------------------------------------
// ファイル名	：EnemyCharactorBasic.cpp
// 内容			：敵キャラクターに共通する処理全般
//-------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharactorBasic.h"
#include "GameFramework/Pawn.h"					//Actorに関する基本的なインクルード
#include "Components/MoveComponent.h"			//移動に関するインクルード
#include "Components/PhysicsCalculator.h"		//重力に関するインクルード
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

class UBoxComponent;

// Sets default values
AEnemyCharactorBasic::AEnemyCharactorBasic()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComponent"));
	RootComponent = BoxComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharactorBasic::BeginPlay()
{
	Super::BeginPlay();

	MoveComp = NewObject<UMoveComponent>(this);
	if (MoveComp)
		MoveComp->Init(this);

	PhysicsCal = NewObject<UPhysicsCalculator>(this);

	FVector PhyForce = FVector(0.0f, 0.0f, 10.0f);
}

// Called every frame
void AEnemyCharactorBasic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//早期リターン
	if (!MoveComp)
	{ 
		return; 
	}

	//移動処理
	MoveComp->Movement(DeltaTime);

	//重力を加算
	PhysicsCal->AddGravity();
}


