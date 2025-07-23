// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ControllableObjectBase.h"
#include "Player/InGameController.h"


AControllableObjectBase::AControllableObjectBase()
{
}

void AControllableObjectBase::Action(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) // ���͂��L���ȏꍇ�i�{�^���������ꂽ�ꍇ�Ȃǁj
	{
		// ����Pawn�𑀍삵�Ă���R���g���[���[��擾
		AController* OwningController = GetController();
		if (OwningController)
		{
			// AInGameController �ɃL���X�g�i��� AInGameController ������PlayerCharacter��Possess���Ă���ꍇ�j
			AInGameController* InGameController = Cast<AInGameController>(OwningController);
			if (InGameController)
			{
				// �R���g���[���[��TogglePossession�֐���Ăяo��
				InGameController->ReturnToOriginalPlayer();
			}
		}
	}
}
