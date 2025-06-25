// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Fireflower.h"
#include "Components/SphereComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/MoveComponent.h"
#include "FunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interface/StateControllable.h"
#include "Objects/ItemBase.h"

void UFireflower::OnCollected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!mOwner)
        return;

    // ���̃A�N�^�[���}���I���ǂ�����m�F
    if (IStateControllable* Mario = Cast<IStateControllable>(OtherActor))
    {
        // �}���I�Ƀp���[�A�b�v�̒ʒm�𑗂�i���݂̓R�����g�A�E�g����Ă���j
        Mario->ChangeState("Fire");

        // �p���[�A�b�v�G�t�F�N�g���ݒ肳��Ă���Ε\��
        if (PowerUpEffect)
        {
            PowerUpEffect->Activate();
        }

        // �A�C�e������������i�R�����g�A�E�g����Ă���j
        mOwner->Destroy();
    }
}

void UFireflower::Init(AItemBase* owner)
{
    // �A�C�e���̏��L�҂��L�����m�F
    if (!owner)
        return;

    // ���L�҂̐ݒ�
    mOwner = owner;
}

void UFireflower::Update(float DeltaTime)
{

}
