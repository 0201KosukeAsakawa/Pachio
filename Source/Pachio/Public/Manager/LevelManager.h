#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class USoundManager;
class UDataTable;
class UBlockDataContainer;
class UObjectManager;

USTRUCT(BlueprintType)
struct FStageData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString ObjectName;
	UPROPERTY(EditAnywhere)
	float Location_X;
	UPROPERTY(EditAnywhere)
	float Location_Y;
	UPROPERTY(EditAnywhere)
	float Location_Z;
	UPROPERTY(EditAnywhere)
	float Rotate_X;
	UPROPERTY(EditAnywhere)
	float Rotate_Y;
	UPROPERTY(EditAnywhere)
	float Rotate_Z;
	UPROPERTY(EditAnywhere)
	float Scale_X;
	UPROPERTY(EditAnywhere)
	float Scale_Y;
	UPROPERTY(EditAnywhere)
	float Scale_Z;
};

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
	void GenerateStage();
	void GenerateBlock();
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USoundManager> SoundManagerClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> ContainerClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObjectManager> ObjectManagerClass;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> StageData;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> BlockData;

	UPROPERTY()
	TObjectPtr<USoundManager> SoundManager;
	UPROPERTY()
	TObjectPtr<UBlockDataContainer> Container;
	UPROPERTY()
	TObjectPtr<UObjectManager>ObjectManager;


	/** 安全なシングルトン用 */
	static TWeakObjectPtr<ALevelManager> Instance;
};