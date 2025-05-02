// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Components/BoxComponent.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

class IStateBase;
class UPlayerDefaultState;
class UInputMappingContext;
class UStateManager;

UCLASS()
class PACHIO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	// ダッシュしているかどうかのフラグ
	bool bIsDashing;
	FVector NewCameraLocation;
	FVector PlayerOldLocation;

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
	void Movement(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void JumpStop(const FInputActionValue& Value);
	void Action(const FInputActionValue& Value);
	void StopAction();

	UPROPERTY()
	UStateManager* manager;

	void ChangeState(FString Tag);

private:
	/** Character用のStaticMesh : Capsule  プレイヤー本体の判定用*/
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Capsule;

	/** Characterの攻撃用のStaticMesh : Box 　(下、踏みつけ攻撃など)
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StompAttackBox;*/

	/*UPROPERTY(VisibleAnywhere, Category = "BoxCollision")
	TSubclassOf<UBoxComponent> StompAttackBox;*/

	// PlayerCharacter.h
	UPROPERTY(VisibleAnywhere, Category = "BoxCollision")
	UBoxComponent* UpperAttackBox;

	/** Character用のStaticMesh : Box 　(上、ブロック破壊など)
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> UpperAttackBox;*/

	// SpringArmをカメラが追従する設定
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/** SpringArmの先端に配置するカメラ */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Special Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAction;
};
