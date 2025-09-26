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
	template<typename T>
	void BindInput(UInputComponent* PlayerInputComponent);

	// ==== 入力マッピング ====

// 使用する入力マッピングコンテキスト（Enhanced Input）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// 各種アクション設定
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	// 各種アクション設定
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IncreaseColorAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DecreaseColorAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShiftArrayRightColorAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShiftArrayLeftColorAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StickAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* Option;
};

// メンバ関数の存在判定テンプレート群
// マクロでHasXXX判定テンプレートを一括生成
#define DEFINE_HAS_MEMBER_FUNCTION(func) \
template<typename T, typename = void> \
struct Has##func : std::false_type {}; \
template<typename T> \
struct Has##func<T, std::void_t<decltype(&T::func)>> : std::true_type {};

// 判定テンプレート生成
DEFINE_HAS_MEMBER_FUNCTION(Jump)
DEFINE_HAS_MEMBER_FUNCTION(Movement)
DEFINE_HAS_MEMBER_FUNCTION(Action)
DEFINE_HAS_MEMBER_FUNCTION(OnMouseScroll)
DEFINE_HAS_MEMBER_FUNCTION(ShiftArrayRightColorMode)
DEFINE_HAS_MEMBER_FUNCTION(ShiftArrayLeftColorMode)
DEFINE_HAS_MEMBER_FUNCTION(OnStickMove)
DEFINE_HAS_MEMBER_FUNCTION(OpenMenu)

// BindInput関数（抜粋例）
template<typename T>
void UPlayerInputComponent::BindInput(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        T* Owner = Cast<T>(GetOwner());
        if (!Owner) return;

        if constexpr (HasJump<T>::value)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, Owner, &T::Jump);
        }

        if constexpr (HasMovement<T>::value)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, Owner, &T::Movement);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, Owner, &T::Movement);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, Owner, &T::Movement);

        }

        if constexpr (HasAction<T>::value)
        {
            EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Triggered, Owner, &T::Action);
        }

        if constexpr (HasOnMouseScroll<T>::value)
        {
            EnhancedInputComponent->BindAction(IncreaseColorAction, ETriggerEvent::Triggered, Owner, &T::OnMouseScroll);
        }

        if constexpr (HasShiftArrayRightColorMode<T>::value)
        {
            EnhancedInputComponent->BindAction(ShiftArrayRightColorAction, ETriggerEvent::Triggered, Owner, &T::ShiftArrayRightColorMode);
        }

        if constexpr (HasShiftArrayLeftColorMode<T>::value)
        {
            EnhancedInputComponent->BindAction(ShiftArrayLeftColorAction, ETriggerEvent::Triggered, Owner, &T::ShiftArrayLeftColorMode);
        }

        if constexpr (HasOnStickMove<T>::value)
        {
            EnhancedInputComponent->BindAction(StickAction, ETriggerEvent::Triggered, Owner, &T::OnStickMove);
        }

        if constexpr (HasOpenMenu<T>::value)
        {
            EnhancedInputComponent->BindAction(Option, ETriggerEvent::Triggered, Owner, &T::OpenMenu);
        }
    }
}

