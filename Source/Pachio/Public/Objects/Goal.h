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

	// �S�[���̃R���W�����R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* GoalBox;

	// �Փˎ��ɌĂ΂��
	UFUNCTION()
	void OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �S�[�����B�����ꂽ���̏�Ԃ�擾
	inline bool IsGoal() const { return isGoal; }

private:
	// �S�[���̏�ԃt���O
	bool isGoal;

	UPROPERTY(EditAnywhere)
	FName nextWorldName;
};