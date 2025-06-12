// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/State/PlayerFireState.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Player/PlayerCharacter.h"
#include "Components/ThrowComponent.h"

UPlayerFireState::UPlayerFireState()
{
	if (!ThrowComp)
		ThrowComp = CreateDefaultSubobject<UThrowComponent>(TEXT("ThrowComponent"));
}

// �X�e�[�g�ɓ���ۂɎ��s����鏈��
bool UPlayerFireState::OnEnter(ACharacter* owner, UWorld* world)
{
	if (owner == nullptr || world == nullptr)
	{
		return false;
	}

	// ����ɏ��L�҂ƃ��[���h��ۑ�
	mOwner = owner;
	pWorld = world;

	// �}�e���A���̐ݒ�i�f�t�H���g�X�e�[�g�p�j
	//if (NewMaterial)
	{
		// �L�����N�^�[������ StaticMeshComponent ��擾
		UStaticMeshComponent* StaticMeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(owner, "StaticMesh");
		UMaterialInterface* N = NewMaterial.LoadSynchronous(); // �񓯊����[�h�ɑΉ�
		if (N != nullptr && StaticMeshComp)
		{
			StaticMeshComp->SetMaterial(0, N); // �}�e���A����X���b�g0�ɓK�p

			//�}�e���A���̃T�C�Y��g��
			StaticMeshComp->SetRelativeScale3D(FVector(1.0, 1.0, 2.0));
		}
	}

	//�R���W�����T�C�Y�ύX
	mOwner->GetCharacterMovement()->Crouch();
	mOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(110.0);

	APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
	if (!aPlayer)
		return false;



	// �ړ����x�̏����l�ݒ�i�X�e�[�g��Ŏg�p�j
	mMoveSpeed = 100.0f;

	return true; // �X�e�[�g�̐؂�ւ�����
}

// �X�e�[�g�̖��t���[���X�V�����i�����_�ł͉�����Ȃ��j
bool UPlayerFireState::OnUpdate(float)
{
	return false;
}

// �X�e�[�g�𗣒E����Ƃ��̏����i�����_�ł͉�����Ȃ��j
bool UPlayerFireState::OnExit(ACharacter*)
{
	return false;
}

// �X�L���{�^�����͎��̏����i�����_�ł͉�����Ȃ��j
bool UPlayerFireState::OnSkill(const FInputActionValue&)
{
	ThrowComp->SetThrownObject(DefaultThrownObjectClass);
	ThrowComp->ThrowObject(FVector(0, 1, 0), 100.0f);
	return true;
}

//�_���[�W��󂯂��Ƃ��̏���
bool UPlayerFireState::TakeDamage()
{
	if (!mOwner)
		return false;

	IStateControllable* is = Cast<IStateControllable>(mOwner);
	if (!is)
		return false;

	is->ChangeState("Default");
	return true;
}
