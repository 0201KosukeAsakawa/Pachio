// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ColorFilterInterface.h"
#include "DataContainer/ColorTargetType.h"
#include "ColorReactiveObject.generated.h"

class UColorReactiveComponent;

UCLASS()
class PACHIO_API AColorReactiveObject : public AActor,public IColorFilterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorReactiveObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void ColorAction(FLinearColor)override;

private:
	// ��������R���|�[�l���g�̃N���X��BP����w��ł���悤��
	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;

	// ���ۂɐ��������R���|�[�l���g�̃|�C���^
	UPROPERTY()
	UColorReactiveComponent* ColorReactiveComponent;
	UPROPERTY(EditAnywhere)
	FLinearColor Color;
	UPROPERTY(EditAnywhere,Category = "Color")
	EColorTargetType ColorTargetType;
};
