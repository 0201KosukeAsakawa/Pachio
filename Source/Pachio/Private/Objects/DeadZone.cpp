// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/DeadZone.h"
#include "Interface/StateControllable.h"
#include "Components/BoxComponent.h"
#include "Components/RespawnComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
ADeadZone::ADeadZone()
{
	PrimaryActorTick.bCanEverTick = true; // ���t���[�� Tick ��ĂԐݒ�

	// �S�[���p�� BoxComponent �𐶐����A���[�g�Ƃ��Đݒ�
	DeadArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DeadArea"));
	RootComponent = DeadArea;

	// �R���W�����ݒ�F
	// �E���������͍s�킸�A�I�[�o�[���b�v���m�̂ݍs��
	DeadArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// �E�S�Ẵ`�����l���ɑ΂��Ė���
	DeadArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	// �EPawn�i�v���C���[�j�Ƃ̃I�[�o�[���b�v�̂ݗL��
	DeadArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// �I�[�o�[���b�v�C�x���g�o�^�F�v���C���[���������� OnGoalOverlap ��Ă�
	DeadArea->OnComponentBeginOverlap.AddDynamic(this, &ADeadZone::OverlapDead);

}

// Called when the game starts or when spawned
void ADeadZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeadZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeadZone::OverlapDead(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	// RespawnComponent を持っているか確認
	if (URespawnComponent* RespawnComp = OtherActor->FindComponentByClass<URespawnComponent>())
	{
		// RespawnComponentがあるなら、リスポーン処理を呼ぶ
		RespawnComp->RespawnOwnerAtInitialLocation();
		return;
	}

	// コンポーネントが無ければ今まで通り State を Dead にする
	if (IStateControllable* IS = Cast<IStateControllable>(OtherActor))
	{
		IS->ChangeState(EPlayerStateType::Dead);
	}
}
