// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/ReadyState.h"
#include "Interface/StateControllable.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"

bool UReadyState::OnEnter(APawn* Owner)
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

bool UReadyState::OnExit(APawn* Owner)
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
	player->ChangeState(EPlayerStateType::Default);
	return true;
}
