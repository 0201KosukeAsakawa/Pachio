// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "PlayerCharacter.generated.h"

class IStateBase;

UCLASS()
class PACHIO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void GenerateState();

private:
	/** Character�p��StaticMesh : Capsule  �v���C���[�{�̂̔���p*/
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Capsule;

	/** Character�̍U���p��StaticMesh : Box �@(���A���݂��U���Ȃ�)*/
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StompAttackBox;

	/** Character�p��StaticMesh : Box �@(��A�u���b�N�j��Ȃ�)*/
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> UpperAttackBox;

	/** Camera��z�u���邽�߂�SpringArm */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/** SpringArm�̐�[�ɔz�u����J���� */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

private:
	UPROPERTY()
	TSubclassOf<IStateBase>CurrentState;

	UPROPERTY()
	TMap<FString, TScriptInterface<IStateBase>> StateMap;
};
