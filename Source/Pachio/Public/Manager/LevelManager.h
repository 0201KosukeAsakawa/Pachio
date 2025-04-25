#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class USoundManager;
class UBlockDataContainer;

UCLASS()
class PACHIO_API ALevelManager : public AActor
{
	GENERATED_BODY()

public:
	ALevelManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Sound")
	static ALevelManager* GetComponent(UObject* WorldContext);

	void PlaySound(FName Category, FName CueName);



private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USoundManager> SoundManagerClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> ContainerClass;


	UPROPERTY()
	TObjectPtr<USoundManager> SoundManager;
	UPROPERTY()
	TObjectPtr<UBlockDataContainer> Container;


	/** 安全なシングルトン用 */
	static TWeakObjectPtr<ALevelManager> Instance;
};