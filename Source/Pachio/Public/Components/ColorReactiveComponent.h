// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorReactiveComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UColorReactiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UColorReactiveComponent();
		
public :
	virtual void Init(UMeshComponent* mesh);
	void SetMyColor(const FLinearColor& FilterColor);
	bool CheckColorMatch(const FLinearColor& FilterColor);

private:
	UFUNCTION(BlueprintCallable)
	bool IsColorMatch(const FLinearColor& FilterColor,const float Tolerance = 0.08f) const;

    UFUNCTION(BlueprintCallable)
    virtual void OnColorMatched(const FLinearColor& FilterColor);  
	virtual void OnColorMismatched(const FLinearColor& FilterColor);
protected:
	UPROPERTY(EditAnywhere)
	FLinearColor Color;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;
};
