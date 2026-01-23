#include "EventSequencer.h"
#include "Components/CameraHandlerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "UE5Coro.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"

AEventSequencer::AEventSequencer()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEventSequencer::BeginPlay()
{
	Super::BeginPlay();

	// カメラハンドラーが設定されていない場合、プレイヤーから自動取得を試みる
	if (!CameraHandler)
	{
		CameraHandler = GetCameraHandler();
	}
}

bool AEventSequencer::PlaySequence(FName SequenceName)
{
	// 既に実行中なら拒否
	if (bIsPlaying)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventSequencer: Already playing sequence '%s'. Cannot start '%s'."),
			*CurrentSequenceName.ToString(), *SequenceName.ToString());
		return false;
	}

	// シーケンスを検索
	const FEventSequence* FoundSequence = Sequences.FindByPredicate([SequenceName](const FEventSequence& Seq)
		{
			return Seq.SequenceName == SequenceName;
		});

	if (!FoundSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventSequencer: Sequence '%s' not found."), *SequenceName.ToString());
		return false;
	}

	// シーケンス実行開始
	CurrentSequenceName = SequenceName;
	bIsPlaying = true;
	bShouldStop = false;

	ExecuteSequence(*FoundSequence);

	return true;
}

void AEventSequencer::StopCurrentSequence()
{
	if (bIsPlaying)
	{
		bShouldStop = true;
		UE_LOG(LogTemp, Log, TEXT("EventSequencer: Stopping sequence '%s'."), *CurrentSequenceName.ToString());
	}
}

UE5Coro::TCoroutine<> AEventSequencer::ExecuteSequence(const FEventSequence& Sequence)
{
	using namespace UE5Coro;

	UE_LOG(LogTemp, Log, TEXT("EventSequencer: Starting sequence '%s'"), *Sequence.SequenceName.ToString());

	// === 開始時の自動設定 ===
	if (Sequence.bPauseGameAtStart)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

	if (Sequence.bDisableInputAtStart)
	{
		APlayerController* PC = GetPlayerController();
		if (PC)
		{
			PC->DisableInput(PC);
		}
	}

	// === 各ステップを順次実行 ===
	for (int32 i = 0; i < Sequence.Steps.Num(); ++i)
	{
		// キャンセル要求チェック
		if (bShouldStop)
		{
			UE_LOG(LogTemp, Log, TEXT("EventSequencer: Sequence '%s' was stopped at step %d."),
				*Sequence.SequenceName.ToString(), i);
			break;
		}

		const FEventStep& Step = Sequence.Steps[i];

		// ステップ実行
		co_await ExecuteStep(Step);
	}

	// === 終了時の自動設定 ===
	if (Sequence.bEnableInputAtEnd)
	{
		APlayerController* PC = GetPlayerController();
		if (PC)
		{
			PC->EnableInput(PC);
		}
	}

	if (Sequence.bUnpauseGameAtEnd)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	// === シーケンス完了 ===
	UE_LOG(LogTemp, Log, TEXT("EventSequencer: Completed sequence '%s'"), *Sequence.SequenceName.ToString());

	FName CompletedName = CurrentSequenceName;
	CurrentSequenceName = NAME_None;
	bIsPlaying = false;

	// デリゲート発火
	OnSequenceComplete.Broadcast(CompletedName);
}

UE5Coro::TCoroutine<> AEventSequencer::ExecuteStep(const FEventStep& Step)
{
	using namespace UE5Coro;

	// ステップの説明があればログ出力
	if (!Step.Description.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("EventSequencer Step: %s"), *Step.Description);
	}

	switch (Step.StepType)
	{
	case EEventStepType::CameraFocus:
	{
		UCameraHandlerComponent* Cam = GetCameraHandler();
		if (Cam)
		{
			co_await Cam->FocusOnLocation(
				Step.TargetLocation,
				Step.Duration,
				Step.MoveSpeed,
				Step.bWaitForComplete,
				Step.bLookAtTarget
			);
		}
		break;
	}

	case EEventStepType::CameraFocusMultiple:
	{
		UCameraHandlerComponent* Cam = GetCameraHandler();
		if (Cam && Step.TargetLocations.Num() > 0)
		{
			co_await Cam->FocusOnMultipleLocations(
				Step.TargetLocations,
				Step.Duration,
				Step.MoveSpeed,
				Step.bLookAtTarget
			);
		}
		break;
	}

	case EEventStepType::CameraZoom:
	{
		UCameraHandlerComponent* Cam = GetCameraHandler();
		if (Cam)
		{
			co_await Cam->ZoomCamera(Step.TargetFOV, Step.Duration);
		}
		break;
	}

	case EEventStepType::CameraShake:
	{
		UCameraHandlerComponent* Cam = GetCameraHandler();
		if (Cam)
		{
			co_await Cam->ShakeCamera(Step.Intensity, Step.Duration);
		}
		break;
	}

	case EEventStepType::CameraReturnToPlayer:
	{
		UCameraHandlerComponent* Cam = GetCameraHandler();
		if (Cam)
		{
			co_await Cam->ReturnToPlayer(Step.MoveSpeed);
		}
		break;
	}

	case EEventStepType::CallActorFunction:
	{
		if (Step.TargetActor && Step.FunctionName != NAME_None)
		{
			// リフレクションで関数を呼び出す
			UFunction* Function = Step.TargetActor->FindFunction(Step.FunctionName);
			if (Function)
			{
				Step.TargetActor->ProcessEvent(Function, nullptr);
				UE_LOG(LogTemp, Log, TEXT("EventSequencer: Called function '%s' on actor '%s'"),
					*Step.FunctionName.ToString(), *Step.TargetActor->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EventSequencer: Function '%s' not found on actor '%s'"),
					*Step.FunctionName.ToString(), *Step.TargetActor->GetName());
			}
		}
		break;
	}

	case EEventStepType::Wait:
	{
		co_await Latent::Seconds(Step.Duration);
		break;
	}

	case EEventStepType::PauseGame:
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		break;
	}

	case EEventStepType::UnpauseGame:
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		break;
	}

	case EEventStepType::DisableInput:
	{
		APlayerController* PC = GetPlayerController();
		if (PC)
		{
			PC->DisableInput(PC);
		}
		break;
	}

	case EEventStepType::EnableInput:
	{
		APlayerController* PC = GetPlayerController();
		if (PC)
		{
			PC->EnableInput(PC);
		}
		break;
	}

	case EEventStepType::PlaySound:
	{
		if (Step.Sound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), Step.Sound);
		}
		break;
	}

	case EEventStepType::SpawnParticle:
	{
		if (Step.ParticleSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Step.ParticleSystem, Step.TargetLocation);
		}
		break;
	}

	case EEventStepType::TriggerColorEvent:
	{
		// 既存のColorManagerシステムと連携
		ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
		if (LevelManager && LevelManager->GetColorManager())
		{
			UE_LOG(LogTemp, Log, TEXT("EventSequencer: Triggering ColorEvent ID='%s'"), *Step.ColorEventID.ToString());
			LevelManager->GetColorManager()->ColorEvent(Step.ColorEventID, Step.ColorEventColor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EventSequencer: LevelManager or ColorManager not found!"));
		}
		break;
	}

	default:
		UE_LOG(LogTemp, Warning, TEXT("EventSequencer: Unknown step type"));
		break;
	}
}

UCameraHandlerComponent* AEventSequencer::GetCameraHandler()
{
	if (CameraHandler)
	{
		return CameraHandler;
	}

	// プレイヤーのPawnからカメラハンドラーを取得
	APlayerController* PC = GetPlayerController();
	if (PC && PC->GetPawn())
	{
		CameraHandler = PC->GetPawn()->FindComponentByClass<UCameraHandlerComponent>();

		if (CameraHandler)
		{
			UE_LOG(LogTemp, Log, TEXT("EventSequencer: Auto-assigned CameraHandler from Player Pawn"));
		}
	}

	return CameraHandler;
}

APlayerController* AEventSequencer::GetPlayerController()
{
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
}