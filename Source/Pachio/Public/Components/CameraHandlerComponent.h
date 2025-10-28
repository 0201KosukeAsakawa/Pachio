// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraHandlerComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * カメラ制御を担当するコンポーネント。
 *
 * グリッド単位でのカメラ移動や、ビュータイプ（サイド／トップ）の切り替えを管理する。
 * スムーズな補間移動やズーム設定、カメラ追従処理などを行う。
 */
UENUM(BlueprintType)
enum class ECameraViewType : uint8
{
	/** 横方向（サイドビュー） */
	GridView UMETA(DisplayName = "Grid View"),

	/** 上方向（トップビュー） */
	CharacterView UMETA(DisplayName = "Character View")
};

/**
 * カメラの挙動やパラメータを制御するコンポーネント。
 *
 * 主にプレイヤーキャラクターにアタッチして使用し、
 * グリッド単位での視点変更やスムーズな追従を実現する。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UCameraHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** コンストラクタ：初期値の設定 */
	UCameraHandlerComponent();

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
	 * @param NewZbaffa Z軸方向の補正値
	 */
	void ApplyCameraSettings(FVector2D NewGridSize, float NewZbaffa);

	/**
	 * カメラ設定を適用（ビュータイプ指定あり）
	 *
	 * @param NewGridSize 新しいグリッドサイズ
	 * @param NewZbaffa Z軸方向の補正値
	 * @param ViewType カメラのビュータイプ（サイド／トップ）
	 */
	void ApplyCameraSettings(FVector2D NewGridSize, float NewZbaffa, ECameraViewType ViewType);

	/**
	 * カメラの表示モードを変更
	 *
	 * @param newMode 新しいカメラビュータイプ
	 */
	void ChangeViewMode(ECameraViewType newMode) { CameraViewType = newMode; }

	/**
	 * 現在のカメラ設定が指定値と一致しているか判定
	 *
	 * @param TargetGridSize 判定対象のグリッドサイズ
	 * @param TargetZbaffa 判定対象のZ補正値
	 *
	 * @return 一致していれば true
	 */
	bool IsParameterMatch(FVector2D TargetGridSize, float TargetZbaffa, ECameraViewType type);

	/**
	 * カメラコンポーネントを取得
	 *
	 * @return カメラコンポーネントのポインタ
	 */
	UCameraComponent* GetCamera() { return Camera; }

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

	/** グリッド1マスのサイズ（X:横幅 / Y:奥行） */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector2D GridSize = FVector2D(7000.f, 3000.f);

	/** Z軸方向のオフセット（高さ補正） */
	UPROPERTY(EditAnywhere, Category = "Grid")
	float Zbaffa = 2000.f;

	/** 現在のグリッド設定 */
	FVector2D CurrentGridSize;

	/** 現在のZ補正値 */
	float CurrentZbaffa;

	/** 現在位置しているグリッド座標（整数値） */
	FIntPoint CurrentGrid = FIntPoint::ZeroValue;

	/** カメラの目標位置（補間対象） */
	FVector TargetCameraLocation;

	/** カメラの補間速度（移動の滑らかさ） */
	float InterpSpeed = 5.f;

	/** 実際のカメラ位置（補間後） */
	FVector CameraLocation;

	/** カメラの回転・位置制御用スプリングアーム */
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/** 実際に描画されるカメラ */
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;
};
