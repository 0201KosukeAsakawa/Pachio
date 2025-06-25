// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaKickedStateState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Enemy/Logic/KoopaShellState.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // ����̈ړ������p�R���|�[�l���g
#include "Components/PhysicsCalculator.h"        // �d�͂Ȃǂ̕������Z�R���|�[�l���g
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"
#include "Logic/Movement/EnemyMoveLogic.h"

// �R�[�p�L�����N�^�[���u�R��ꂽ�v��Ԃɓ��鎞�̏���
bool UKoopaKickedStateState::OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet, const EEnemyCategory materialID)
{
    // �I�[�i�[�A���[���h�A���W�b�N�R���|�[�l���g�������ȏꍇ�͏�����I��
    if (!owner || !world || !LogicComponet)
        return false;

    mOwner = owner;
    logicComponent = LogicComponet;

    // �W�����v�\�ɐݒ�
    mOwner->SetCanJamp(true);

    // �ړ��R���|�[�l���g�̏�����
    MoveComp = NewObject<UMoveComponent>(mOwner);
    // �U���R���|�[�l���g�̏�����
    Attack = NewObject<UAttackComponent>(mOwner);
    mOwner->SetHp(1.0f); // HP ��1�ɐݒ�

    // Actor �I�u�W�F�N�g��L���X�g���A�K�v�ȃR���|�[�l���g�̏�����
    AActor* actor = Cast<AActor>(mOwner);
    if (!actor || !MoveComp || !Attack || !Attack->Init(world, "DamageOnly"))
        return false;

    // �U���f�[�^�̐ݒ�i�G�L�����N�^�[�ɑ΂���U���A���Ȃ��U���j
    Attack->SetAttackData(EAttackType::Indiscriminate, EBreakLevel::Breakable);

    // �L�����N�^�[�̈ړ��R���|�[�l���g�̏�����
    MoveComp->Init(actor, NewObject<UEnemyMoveLogic>(this), 500.0f,FVector(0,0,0));

    // �����v�Z�R���|�[�l���g�̏������i�d�͂̓K�p�Ȃǁj
    PhysicsCal = NewObject<UPhysicsCalculator>(actor);

    // ���b�V����������Ώ�����I��
    if (!owner->GetMesh())
        return false;

    // ���b�V���̃R���W�����𖳌��ɂ��āA�U����󂯂�܂ŏՓ˂��Ȃ��悤�ɐݒ�
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // �V�����}�e���A����쐬���Đݒ�
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(world)->GetEnemyContainer()->CreateMaterial(world, materialID);
    if (!newMaterial)
        return false;

    owner->GetMesh()->SetMaterial(0, newMaterial); // ���b�V���ɐV�����}�e���A����ݒ�

    return true; // �����������������ꍇ
}

bool UKoopaKickedStateState::OnUpdate(float DeltaTime)
{
    // �ړ��R���|�[�l���g��������Ώ�����s��Ȃ�
    if (!MoveComp || !mOwner || !logicComponent)
    {
        return false;
    }

    // ���C�L���X�g��s���A�i�s�����ɏ�Q�������邩�m�F
    FVector Start = mOwner->GetActorLocation(); // ���݂̈ʒu
    FVector End = Start + (mOwner->GetActorForwardVector() * 100.0f); // �i�s������100���j�b�g��܂Ń��C���΂��i���������\�j

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(mOwner);  // ���g�𖳎�����

    // ���C�L���X�g�𔭎�
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    // ���C�L���X�g�ŉ����ɓ��������ꍇ
    if (bHit)
    {
        // �Փ˂����A�N�^�[��擾
        AActor* hitActor = HitResult.GetActor();
        // �U���R���|�[�l���g��������Ώ������Ȃ�
        if (!Attack)
            return false;

        // �U����������s
        Attack->PerformAttack(hitActor);
    }

    // �ړ������i�R���Ĉړ�����j
    FVector v = MoveComp->Movement(DeltaTime, mOwner);
    FVector m = v - mOwner->GetActorLocation();
    mOwner->SetActorLocation(v);


    // �L�����N�^�[�����S�����ꍇ�A�b����ԂɑJ��
    if (mOwner->IsDead())
    {
        UKoopaShellState* nextState = NewObject<UKoopaShellState>(mOwner);
        logicComponent->ChangeState(nextState, mOwner); // �b����ԂɑJ��
    }

    return true;
}


// ��ԏI�����̏���
bool UKoopaKickedStateState::OnExit()
{
    return true;
}

// �Փˎ��̏���
bool UKoopaKickedStateState::OnOverlap(AActor* hitActor)
{

    //// �U���R���|�[�l���g��������Ώ������Ȃ�
    //if (!Attack)
    //    return false;

    //// �U����������s
    //Attack->PerformAttack(hitActor);
    return true;
}

// �ړ������̐ݒ�
void UKoopaKickedStateState::SetDirection(FVector d)
{
    // �ړ��R���|�[�l���g�Ɉړ�������ݒ�ta
    MoveComp->SetDirection(d);
}
