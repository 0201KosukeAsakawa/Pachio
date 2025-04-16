#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundManager.h"
#include "InGameSoundManager.generated.h"

UCLASS()
class PACHIO_API AInGameSoundManager : public AActor
{
	GENERATED_BODY()

public:
	AInGameSoundManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Sound")
	static AInGameSoundManager* Get(UObject* WorldContext);

	void PlaySound(FName Category, FName CueName);

private:
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundManager* SoundManager;

	/** 安全なシングルトン用 */
	static TWeakObjectPtr<AInGameSoundManager> Instance;
};