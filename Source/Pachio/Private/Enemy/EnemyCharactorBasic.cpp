//-------------------------------------------
// ファイル名	：EnemyCharactorBasic.cpp
// 内容			：敵キャラクターに共通する処理全般
//-------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharactorBasic.h"
#include "GameFramework/Actor.h"              // Actorに関する基本的なインクルード
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

class UBoxComponent;

// Sets default values
AEnemyCharactorBasic::AEnemyCharactorBasic()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	FVector ForwardDirection = GetActorForwardVector();

	// 前方に移動するための入力をAddMovementInputで適用
	
	AddActorWorldOffset(ForwardDirection, true);

	UE_LOG(LogTemp, Warning, TEXT("Success Function!!"));

}

void AEnemyCharactorBasic::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

