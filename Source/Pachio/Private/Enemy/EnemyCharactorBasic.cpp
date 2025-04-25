//-------------------------------------------
// ファイル名	：EnemyCharactorBasic.cpp
// 内容			：敵キャラクターに共通する処理全般
//-------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharactorBasic.h"
#include "GameFramework/Pawn.h"              // Actorに関する基本的なインクルード
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

	MeshComponent->OnComponentHit.AddDynamic(this, &AEnemyCharactorBasic::OnHit);
}

// Called when the game starts or when spawned
void AEnemyCharactorBasic::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemyCharactorBasic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//移動処理
	MoveTo(DeltaTime);

}

void AEnemyCharactorBasic::MoveTo(float _deltaTime)
{
	// 移動量を設定（例：X方向に100ユニット/秒）
	FVector MoveAmount(100.0f, 0.0f, 0.0f);

	// DeltaTime を使ってフレームごとに移動量を計算
	FVector DeltaMove = MoveAmount * _deltaTime;

	// アクターを移動
	AddActorWorldOffset(DeltaMove, true);  // 衝突を有効にしたい場合は bSweep = true を指定
}

void AEnemyCharactorBasic::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

