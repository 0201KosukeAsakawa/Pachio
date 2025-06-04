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
	void a(const FLinearColor& FilterColor);
private:
	UFUNCTION(BlueprintCallable)
    // フィルター色と一致するか
	bool IsColorMatch(const FLinearColor& FilterColor,const float Tolerance = 0.05f) const;

    // 色が一致したときの動作（派生クラスで定義）
    UFUNCTION(BlueprintCallable)
    virtual void OnColorMatched();  // 継承先で override

protected:
	UPROPERTY(EditAnywhere)
	FLinearColor Color;

	bool bColorMatch = false;
};
