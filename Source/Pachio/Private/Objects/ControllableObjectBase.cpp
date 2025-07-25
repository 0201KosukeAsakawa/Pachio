// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ControllableObjectBase.h"
#include "GameFramework/Character.h"
#include "Player/InGameController.h"


AControllableObjectBase::AControllableObjectBase()
{
}

void AControllableObjectBase::hoge(ACharacter* player)
{
	// このPawnを操作しているコントローラーを取得
	AController* OwningController = player->GetController();
	if (OwningController)
	{
		// AInGameController にキャスト（もし AInGameController がこのPlayerCharacterをPossessしている場合）
		AInGameController* InGameController = Cast<AInGameController>(OwningController);
		if (InGameController)
		{
			// コントローラーのTogglePossession関数を呼び出す
			InGameController->TogglePossession(this);
		}
	}
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
