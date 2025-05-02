#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Goal.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API AGoal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGoal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ゴールのコリジョンコンポーネント
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* GoalBox;

	// 衝突時に呼ばれる
	UFUNCTION()
	void OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ゴールが達成されたかの状態を取得
	inline bool IsGoal() const { return isGoal; }

private:
	// ゴールの状態フラグ
	bool isGoal;

	UPROPERTY(EditAnywhere)
	FName nextWorldName;
};