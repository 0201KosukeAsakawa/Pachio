// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorReactiveComponent.generated.h"
struct FHSLColor
{
	float H; // 0.0?1.0
	float S; // 0.0?1.0
	float L; // 0.0?1.0
};
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
	void ApplyColorToMaterial(FLinearColor InColor);
	bool CheckColorMatch(const FLinearColor& FilterColor);	
	UFUNCTION(BlueprintCallable)
	bool IsColorMatch(const FLinearColor& FilterColor,const float Tolerance = 0.08f) const;
	FLinearColor GetComplementaryColor(const FLinearColor& InColor);


private:
    UFUNCTION(BlueprintCallable)
    virtual void OnColorMatched(const FLinearColor& FilterColor);  
	virtual void OnColorMismatched(const FLinearColor& FilterColor);
protected:
	UPROPERTY(EditAnywhere)
	FLinearColor CurrentColor;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;
};
