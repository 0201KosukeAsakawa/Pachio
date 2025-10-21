// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/StateManagerBase.h"
#include "Components/Player/PlayerStateComponent.h"

// Sets default values for this component's properties
UStateManagerBase::UStateManagerBase() :
										CurrentState(nullptr)
										,mOwner(nullptr)
{

}

void UStateManagerBase::Init_Implementation(APawn* Owner, UWorld* World)
{
	if (Owner == nullptr)
		return;
	mOwner = Owner;
	// 初期状態を "Default" に設定
	Execute_ChangeState(this, EPlayerStateType::Default);
	// PlayerCharacter（やPawn）がACharacter型なら、SkeletalMeshを持っている
	ACharacter* Character = Cast<ACharacter>(Owner);

	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("URedStateManagerBase::Init_Implementation - Owner is not a Character"));
		return;
	}

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("URedStateManagerBase::Init_Implementation - Character has no Mesh"));
		return;
	}

	// 🔸 StateManagerが保持するメッシュとアニメBPを適用
	if (FormMesh)
	{
		MeshComp->SetSkeletalMesh(FormMesh);
	}

	if (FormAnimBP)
	{
		MeshComp->SetAnimInstanceClass(FormAnimBP);
	}

	UE_LOG(LogTemp, Log, TEXT("Red form mesh and animation blueprint applied successfully."));
}

void UStateManagerBase::Update_Implementation(float DeltaTime)
{
}

UPlayerStateComponent* UStateManagerBase::ChangeState_Implementation(EPlayerStateType NextStateTag)
{
	if (StateClassMap.IsEmpty() || !StateClassMap.Contains(NextStateTag) || !mOwner)
		return nullptr;

	TSubclassOf<UPlayerStateComponent> StateClass = StateClassMap[NextStateTag];

	// 既存ステートを終了
	if (CurrentState)
	{
		CurrentState->OnExit(mOwner);
		CurrentState->ConditionalBeginDestroy(); // メモリ解放（必要に応じて）
		CurrentState = nullptr;
	}

	// 新しいステートを生成
	CurrentState = NewObject<UPlayerStateComponent>(mOwner, StateClass);
	if (!CurrentState)
		return nullptr;

	CurrentState->OnEnter(mOwner, GetWorld());
	return CurrentState;
}

bool UStateManagerBase::IsStateMatch_Implementation(EPlayerStateType StateTag)
{
	TSubclassOf<UPlayerStateComponent> StateClass = StateClassMap.FindRef(StateTag);

	if (StateClass == nullptr || CurrentState == nullptr)
	{
		return false;
	}

	return CurrentState->IsA(StateClass);
}

