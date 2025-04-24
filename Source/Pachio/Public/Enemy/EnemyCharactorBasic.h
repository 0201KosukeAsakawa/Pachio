//-------------------------------------------
// ファイル名	：EnemyCharactorBasic.h
// 内容			：敵キャラクターに共通する処理全般
//-------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCharactorBasic.generated.h"

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

	UFUNCTION()
	void MoveTo(float _deltaTime);		//移動処理

protected:
	UFUNCTION(BlueprintCallable)
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
