// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaAliveState.h"
#include "Enemy/Logic/KoopaShellState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // ����̈ړ������p�R���|�[�l���g
#include "Components/PhysicsCalculator.h"        // �d�͂Ȃǂ̕������Z�R���|�[�l���g
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"
#include "Logic/Movement/EnemyMoveLogic.h"

// �R�[�p�L�����N�^�[���u�����v��Ԃɓ��鎞�̏���
bool UKoopaAliveState::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, UEnemyStateComponent* LogicComponet, const EEnemyCategory materialID)
{
    // �I�[�i�[�A���x���A���W�b�N�R���|�[�l���g�������ȏꍇ�͏�����I��
    if (!owner || !currentLevel || !LogicComponet)
        return false;

    mOwner = owner;
    logicComponent = LogicComponet;

    // �W�����v�\�ɐݒ�
    mOwner->SetCanJamp(true);

    // �ړ��R���|�[�l���g�������
    MoveComp = NewObject<UMoveComponent>(mOwner);
    // �U���R���|�[�l���g�̏�����
    Attack = NewObject<UAttackComponent>(mOwner);
   
    // �����v�Z�p�̃R���|�[�l���g�������
    AActor* actor = Cast<AActor>(mOwner);
    if (!actor || !MoveComp || !Attack || !Attack->Init(currentLevel, "DamageOnly"))
        return false;
    Attack->SetAttackData(EAttackType::Enemy, EBreakLevel::Unbreakable);
    // �L�����N�^�[�̈ړ��R���|�[�l���g������
    MoveComp->Init(actor, NewObject<UEnemyMoveLogic>(this),100.0f,FVector(0,-1,0));
    //MoveComp->SetSpeed(10.0f); // �ړ����x�ݒ�

    // �����v�Z�R���|�[�l���g��������i�d�͂Ȃǂ̓K�p�j
    PhysicsCal = NewObject<UPhysicsCalculator>(actor);

    // ���b�V����������Ώ�����I��
    if (!owner->GetMesh())
        return false;

    // ���b�V���̃R���W�����𖳌��ɂ���i�U����󂯂�܂ŏՓ˂��Ȃ��悤�ɂ���j
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // �V�����}�e���A����쐬���Đݒ�
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(currentLevel)->GetEnemyContainer()->CreateMaterial(currentLevel, EEnemyCategory::Koopa /*"Koopa"*/ , "Default");
	if (!newMaterial)
		return false;

    owner->GetMesh()->SetMaterial(0, newMaterial); // ���b�V���ɐV�����}�e���A����ݒ�

    // ����HP��ݒ�
    mOwner->SetHp(1.0f);

    return true; // �����������������ꍇ
}

// ������ԂŖ��t���[�����s����鏈��
bool UKoopaAliveState::OnUpdate(float DeltaTime)
{
    // �ړ��R���|�[�l���g��������Ώ�����s��Ȃ�
    if (!MoveComp || !mOwner || !logicComponent)
    {
        return false;
    }

    // �ړ������i�ǐՂ�p�g���[���Ȃǁj
    FVector v = MoveComp->Movement(DeltaTime, mOwner);
    FVector m = v - mOwner->GetActorLocation();
    mOwner->SetActorLocation(v);

    // �L�����N�^�[�����S�����ꍇ�A���̏�ԁi�b����ԁj�ɑJ��
    if (mOwner->IsDead())
    {
        UKoopaShellState* nextState = NewObject<UKoopaShellState>(mOwner);
        logicComponent->ChangeState(nextState, mOwner); // �b����ԂɑJ��
    }

    return true;
}

// ��ԏI�����̏����i���ɕK�v�ȏ����͂Ȃ��j
bool UKoopaAliveState::OnExit()
{
    return true;
}

// �Փˎ��̏���
bool UKoopaAliveState::OnOverlap(AActor* hitActor)
{
    // �U���R���|�[�l���g��������Ώ������Ȃ�
    if (!Attack)
        return false;

    // �U����������s
    Attack->PerformAttack(hitActor);
    return true;
}
