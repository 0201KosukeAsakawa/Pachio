/**
 * カメラ制御を担当するコンポーネント。
 *
 * グリッド単位でのカメラ移動や、ビュータイプ（CharacterView／GridView）の切り替えを管理する。
 * スムーズな補間移動やズーム設定、カメラ追従処理、イベント演出などを行う。
 */

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "UE5Coro.h"
#include "CameraHandlerComponent.generated.h"

class UCameraComponent;

using namespace UE5Coro;
/**
 * カメラのビュータイプ
 */
UENUM(BlueprintType)
enum class ECameraViewType : uint8
{
	/** グリッドビュー */
	GridView UMETA(DisplayName = "Grid View"),

	/** キャラクタービュー */
	CharacterView UMETA(DisplayName = "Character View")
};

/**
 * カメラの挙動やパラメータを制御するコンポーネント。
 *
 * 主にプレイヤーキャラクターにアタッチして使用し、
 * グリッド単位での視点変更やスムーズな追従、イベント演出を実現する。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UCameraHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** コンストラクタ：初期値の設定 */
	UCameraHandlerComponent();

	UFUNCTION(BlueprintCallable, Category = "Camera Test")
	void TestEventCamera() { TestEventCameraCoroutine(); }

	UE5Coro::TCoroutine<> TestEventCameraCoroutine();

	/**
	 * カメラ初期化処理
	 *
	 * @param RootComponent カメラの基準となるルートコンポーネント
	 */
	void Init(TObjectPtr<USceneComponent> RootComponent);

	/**
	 * 毎フレーム更新処理
	 *
	 * @param DeltaTime フレーム間の経過時間
	 * @param TickType Tickの種類
	 * @param ThisTickFunction Tick関数情報
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * カメラ設定を適用（ビュータイプ指定なし）
	 *
	 * @param NewGridSize 新しいグリッドサイズ
	 * @param NewCameraDistance カメラ距離（X軸オフセット）
	 */
	void ApplyCameraSettings(FVector2D NewGridSize, float NewCameraDistance);

	/**
	 * カメラ設定を適用（ビュータイプ指定あり）
	 *
	 * @param NewGridSize 新しいグリッドサイズ
	 * @param NewCameraDistance カメラ距離（X軸オフセット）
	 * @param ViewType カメラのビュータイプ
	 */
	void ApplyCameraSettings(FVector2D NewGridSize, float NewCameraDistance, ECameraViewType ViewType);

	/**
	 * カメラの表示モードを変更
	 *
	 * @param NewMode 新しいカメラビュータイプ
	 */
	void ChangeViewMode(ECameraViewType NewMode) { CameraViewType = NewMode; }

	/**
	 * 現在のカメラ設定が指定値と一致しているか判定
	 *
	 * @param TargetGridSize 判定対象のグリッドサイズ
	 * @param TargetCameraDistance 判定対象のカメラ距離
	 * @param TargetType 判定対象のビュータイプ
	 *
	 * @return 一致していれば true
	 */
	bool IsParameterMatch(FVector2D TargetGridSize, float TargetCameraDistance, ECameraViewType TargetType);

	/**
	 * カメラコンポーネントを取得
	 *
	 * @return カメラコンポーネントのポインタ
	 */
	UCameraComponent* GetCamera() { return Camera; }

	// ========== イベント演出用コルーチン ==========

	/**
	 * イベント位置にカメラをフォーカス(コルーチン版)
	 *
	 * @param EventLocation イベント発生位置
	 * @param FocusDuration 注視時間(秒)
	 * @param MoveSpeed 移動速度
	 * @param bWaitForComplete 移動完了を待つか
	 */
	TCoroutine<> FocusOnLocation(
		FVector EventLocation,
		float FocusDuration = 2.0f,
		float MoveSpeed = 3.0f,
		bool bWaitForComplete = true
	);

	/**
	 * 複数地点を順番にフォーカス
	 *
	 * @param Locations フォーカス位置の配列
	 * @param DurationPerLocation 各地点の注視時間
	 * @param MoveSpeed 移動速度
	 */
	TCoroutine<> FocusOnMultipleLocations(
		TArray<FVector> Locations,
		float DurationPerLocation = 2.0f,
		float MoveSpeed = 3.0f
	);

	/**
	 * プレイヤー位置に戻る(コルーチン版)
	 *
	 * @param MoveSpeed 移動速度
	 */
	TCoroutine<> ReturnToPlayer(float MoveSpeed = 3.0f);

	/**
	 * カメラズーム演出
	 *
	 * @param TargetFOV 目標FOV
	 * @param Duration ズーム時間
	 */
	TCoroutine<> ZoomCamera(float TargetFOV, float Duration = 1.0f);

	/**
	 * カメラシェイク演出
	 *
	 * @param Intensity 揺れの強さ
	 * @param Duration 揺れの時間
	 */
	TCoroutine<> ShakeCamera(float Intensity = 10.0f, float Duration = 0.5f);

	/**
	 * イベント演出中かどうか
	 *
	 * @return イベント演出中ならtrue
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Event")
	bool IsInEventMode() const { return bIsInEventMode; }

private:
	/**
	 * カメラ位置を更新（補間によるスムーズ移動）
	 *
	 * @param DeltaTime 経過時間
	 */
	void UpdateCameraPosition(float DeltaTime);

	/**
	 * ビュータイプに応じてカメラの位置を設定
	 *
	 * @param ViewType カメラビュータイプ
	 */
	void SetCameraLocation(ECameraViewType ViewType);

private:
	/** 現在のカメラ表示タイプ */
	UPROPERTY(EditAnywhere, Category = "Grid")
	ECameraViewType CameraViewType = ECameraViewType::CharacterView;

	/** グリッド1マスのサイズ（X:横幅 / Y:高さ） */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector2D GridSize = FVector2D(7000.f, 3000.f);

	/** カメラのX軸方向距離（奥行き） */
	UPROPERTY(EditAnywhere, Category = "Grid")
	float CameraDistance = 2000.f;

	/** 現在位置しているグリッド座標（整数値） */
	FIntPoint CurrentGrid = FIntPoint::ZeroValue;

	/** カメラの目標位置（補間対象） */
	FVector TargetCameraLocation;

	/** カメラの補間速度（移動の滑らかさ） */
	UPROPERTY(EditAnywhere, Category = "Camera")
	float InterpSpeed = 3.0f;

	/** 実際に描画されるカメラ */
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	// ========== イベント演出用変数 ==========

	/** イベント演出中フラグ */
	bool bIsInEventMode = false;

	/** 元のFOV(ズーム用) */
	float OriginalFOV = 90.0f;

	/** イベント前のターゲット位置 */
	FVector PreEventTargetLocation;
};