//-------------------------------------------
// ファイル名	：EnemyCharactorBasic.cpp
// 内容			：敵キャラクターに共通する処理全般
//-------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharactorBasic.h"

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

// Called to bind functionality to input
void AEnemyCharactorBasic::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharactorBasic::MoveTo(float _deltaTime)
{
	// キャラクターの前方向に移動
    FVector Forward = GetActorForwardVector();					//GetActorForwardVector()：アクター（AActor）が向いている方向（前方向）を示す単位ベクトル
    AddMovementInput(Forward, MoveSpeed * _deltaTime);			//AddMovementInput：キャラクターの移動処理を簡素化、物理計算を自動的に処理、
																//					移動のための入力をCharacterMovementComponentに直接渡すメリットがある。
	//デバックログ表示
	UE_LOG(LogTemp, Warning, TEXT("Success Function!!"));
}
