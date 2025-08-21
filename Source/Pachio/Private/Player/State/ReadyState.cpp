// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/ReadyState.h"
#include "Interface/StateControllable.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"

bool UReadyState::OnEnter(ACharacter* Owner, UWorld* World)
{
	if (GetWorld() == nullptr)
		return false;

	ALevelManager* manager = ALevelManager::GetInstance(GetWorld());
	if (manager == nullptr)
		return false;

	UUIManager* uiManager = manager->GetUIManager();

	if (uiManager == nullptr)
		return false;

	uiManager->ShowWidget(EWidgetCategory::Menu, "Start");

	return true;
}


bool UReadyState::OnUpdate(float DeltaTime)
{
	return true;
}

bool UReadyState::OnExit(ACharacter* Owner)
{
	if (GetWorld() == nullptr)
		return false;

	ALevelManager* manager = ALevelManager::GetInstance(GetWorld());
	if (manager == nullptr)
		return false;

	UUIManager* uiManager = manager->GetUIManager();

	if (uiManager == nullptr)
		return false;

	uiManager->HideCurrentWidget(EWidgetCategory::Menu, "Start");

	return true;
}

bool UReadyState::OnSkill(const FInputActionValue& Input)
{
	if (GetWorld() == nullptr)
		return false;
	IStateControllable* player = Cast<IStateControllable>(GetOwner());
	if (player == nullptr)
		return false;
	player->ChangeState("Dafault");
	return true;
}
