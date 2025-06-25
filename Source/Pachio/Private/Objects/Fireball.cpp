//------------------------------------------
// �t�@�C�����FFireball.cpp
// ������e�F�����鋅�h���N���X
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Fireball.h"
#include "Components/PhysicsCalculator.h"

AFireball::AFireball()
{
	PrimaryActorTick.bCanEverTick = true;
 
	//UPhysicsCalculator�𐶐��E�A�^�b�`��s��
	Physics = CreateDefaultSubobject<UPhysicsCalculator>(TEXT("PhysicsCalculator"));
	if (!IsValid(Physics))
	{
		UE_LOG(LogTemp, Error, TEXT("Physics component not found!"));
	}
}

void AFireball::BeginPlay()
{
	Super::BeginPlay();		//�e�N���X��BeginPlay��Ăяo��

}

// ���t���[���Ă΂��
void AFireball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ThrowObject(Direction, Force);

}

//Function to throw things
void AFireball::ThrowObject(FVector AimDirection, float Impulse)
{
	if (!IsValid(Physics))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Not Physics"));
		return;
	}

	//������͂������
	Physics->AddForce(Direction, Force, true);
	UE_LOG(LogTemp, Warning, TEXT("Succeeded PhysicsAddForce"));
	
	UE_LOG(LogTemp, Warning, TEXT("Succeeded PhysicsAddGravity"));

}