// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InvincibilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UInvincibilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInvincibilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartInvincible();
	inline bool IsInvincible()const { return bIsInvincible; }
private:
	void ToggleVisibility();  // メッシュの表示/非表示を切り替える
	void UpdateInvincible(float DeltaTime);  // 無敵時間の管理
		
private:
	//無敵時間中フラグ
	bool bIsInvincible = false;
	bool bIsVisible = true; //無敵時間時の点滅フラグ
	float InvincibleTime = 0.0f; //無敵時間の計測タイマー
	float MaxInvincibleTime = 2.0f; // 無敵時間を2秒と仮定
	FTimerHandle BlinkTimerHandle;
};
