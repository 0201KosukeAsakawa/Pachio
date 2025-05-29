#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/IDamageable.h"
#include "Enemy/EnemyCategory.h"
#include "EnemyCharacter.generated.h"

class UBoxComponent;
class UMoveComponent;
class UPhysicsCalculator;
class UEnemyStateComponent;

UCLASS()
class PACHIO_API AEnemyCharacter : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// 初期化関数 (StateID, DropItemID, OptionalなMaterialID)
	void Init(const EEnemyCategory stateID,const EEnemyCategory materialID);
	/**
 * ダメージを受ける処理を実装する関数
 * @param Data 攻撃の詳細情報（攻撃種別や属性など）
 * @param damage 攻撃による数値的なダメージ（省略可能、Dataからも判定可能な場合）
 * @return true：ダメージが正常に処理された / false：何らかの理由で処理されなかった
 */
	bool TakeDamage(FAttackData Data,const float damage = 0 , const AActor* = nullptr)override;

	/**
	 * 現在のアクターが死亡状態かどうかを返す
	 * @return true：死亡している / false：生存している
	 */
	bool IsDead() const override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	inline UStaticMeshComponent* GetMesh() { return meshComponent; }
	inline UBoxComponent* GetAttackCollision() { return AttackCollision; }
	inline void SetHp(float hp) { HP = hp; }
	inline void SetCanJamp(bool isStomp) { canStomp = isStomp; }
	inline bool CanBeStomped()const { return canStomp; }
private:
	UPROPERTY()
	UBoxComponent* AttackCollision;

	UPROPERTY()
	UStaticMeshComponent* meshComponent;

private:
	UPROPERTY()
	UEnemyStateComponent* Logic;

	UPROPERTY(EditAnywhere)
	float HP = 1.0f;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 0.0f;

	UPROPERTY(EditAnywhere)
	int Score = 100;

	UPROPERTY(EditAnywhere)
	EEnemyCategory LogicID;

	UPROPERTY(EditAnywhere)
	EEnemyCategory MaterialID;

	bool canStomp;
};
