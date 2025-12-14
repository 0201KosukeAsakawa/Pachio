// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "PlayerInputComponent.generated.h"

UCLASS( Blueprintable )
class PACHIO_API UPlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	void Init(TObjectPtr<AController>);
	// Sets default values for this component's properties
	UPlayerInputComponent();

public:
	void BindInput(UInputComponent* PlayerInputComponent);

private:

	// -------- 中継関数群 --------
	void HandleMove(const FInputActionValue& Value);
	void HandleJump(const FInputActionValue& Value);
	void HandleAction(const FInputActionValue& Value);
	void HandleIncreaseColor(const FInputActionValue& Value);
	void HandleShiftColorRight(const FInputActionValue& Value);
	void HandleShiftColorLeft(const FInputActionValue& Value);
	void HandleStick(const FInputActionValue& Value);
	void HandleOpenMenu(const FInputActionValue& Value);


private:
	// ==== 入力マッピング ====

// 使用する入力マッピングコンテキスト（Enhanced Input）
	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// 各種アクション設定
	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	// 各種アクション設定
	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IncreaseColorAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DecreaseColorAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShiftArrayRightColorAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShiftArrayLeftColorAction;

	UPROPERTY(EditAnywhere,   Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StickAction;

    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* Option;


	/** 入力の委譲先 */
	UPROPERTY()
	TScriptInterface<IControllableInterface> ControllableTarget = nullptr;
};
