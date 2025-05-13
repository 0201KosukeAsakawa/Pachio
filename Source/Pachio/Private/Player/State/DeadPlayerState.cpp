// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/DeadPlayerState.h"
#include "Interface/StateControllable.h"
#include "Kismet/GameplayStatics.h" // �����ǉ�

bool UDeadPlayerState::OnEnter(ACharacter* Owner, UWorld* World)
{
	if (!Owner || !World)
		return false;

	IStateControllable* is = Cast<IStateControllable>(Owner);
	if (!is)
		return false;

	is->ChangeState("Default");

	UGameplayStatics::OpenLevel(this, "NewWorld");

	return true;
}
