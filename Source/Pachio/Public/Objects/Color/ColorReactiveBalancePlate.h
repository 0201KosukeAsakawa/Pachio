#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorReactiveBalancePlate.generated.h"

UCLASS()
class PACHIO_API AColorReactiveBalancePlate : public AActor
{
    GENERATED_BODY()

public:
    AColorReactiveBalancePlate();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;    
    void SetLocation(FVector next);
    void ResetLocation();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MinPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MaxPosition;

    FVector GetMinPosition() const { return MinPosition; }
    FVector GetMaxPosition() const { return MaxPosition; }

private:
    FVector mDefaultPosition;
    FVector v;
    bool IsReached(FVector v1, FVector v2);
};
