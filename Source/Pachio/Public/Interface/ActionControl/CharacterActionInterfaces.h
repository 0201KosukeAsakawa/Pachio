#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterActionInterfaces.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControllableMover : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACHIO_API IControllableMover
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Movement(const FInputActionValue& Value);
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControllableJumper : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PACHIO_API IControllableJumper
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Jump(const FInputActionValue& Value);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControllableAbility : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PACHIO_API IControllableAbility
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Action(const FInputActionValue& Value);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UColorModeController : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PACHIO_API IColorModeController
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void ChangeColor(float);

	// カラーモードを1つ右にシフト
	virtual void ShiftArrayRightColorMode();

	// カラーモードを1つ左にシフト
	virtual void ShiftArrayLeftColorMode();
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStickAction : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PACHIO_API IStickAction
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnStickMove(const FInputActionValue& Value);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOptionAction : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PACHIO_API IOptionAction
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OpenMenu(const FInputActionValue& Value);
};
