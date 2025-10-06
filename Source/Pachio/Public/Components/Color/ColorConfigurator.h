#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorConfigurator.generated.h"

class ALevelManager;
class ANiagaraActor;
class UBeatScalerComponent;
class UColorReactiveComponent;
class UColorManager;



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorConfigurator : public UActorComponent
{
	GENERATED_BODY()

public:
	UColorConfigurator();

	// ====== 初期化処理 ======
	/** コンポーネント初期化処理（呼び出し用） */
	virtual void Init();
	/** 色関連のロジックを初期化 */
	virtual void InitializeColorLogic();
	/** カラーマネージャにこのターゲットを登録 */
	virtual void RegisterToColorManager();
	/** 材質(Material)に初期色を設定 */
	virtual void SetupMaterial();

	// ====== ビート検出 ======
	/** ビート検出時のアニメーション再生 */
	UFUNCTION()
	virtual void PlayBeatAnimation();

	// ====== 色に応じたアクション ======
	/** 色に応じたアクション処理を実行 */
	virtual void ColorAction(FLinearColor InColor, FEffectMatchResult MatchResult);

	// ====== 色操作 ======
	/** 色を初期色にリセット */
	virtual void ResetColor(FEffectMatchResult MatchResult);
	/** 新しい色を設定 */
	virtual void SetColor(FLinearColor InColor, FEffectMatchResult MatchResult);
	/** 材質に色を適用 */
	virtual void ApplyColorToMaterial(FLinearColor InColor);
	/** 内部的に現在の色を更新 */
	void SetCurrentColor(FLinearColor InColor);
	/** 色一致フラグを設定 */
	void SetColorMatch(bool bInColorMatch);
	/** 選択モードを設定 */
	void SetSelectMode(bool bIsSelect);
	/** 色変更ロックを切り替え */
	void ChangeLock(bool bLock) { bIsColorVariable = bLock; }

	// ====== 色判定 ======
	/** 初期色と一致しているかを確認 */
	virtual bool IsColorChange() const;
	/** 指定色と一致しているかを確認 */
	virtual bool IsColorChange(FLinearColor InColor) const;
	/** 内部フラグによる色一致確認 */
	bool IsColorMatch() const;
	/** 2つの色の一致確認（許容誤差あり） */
	bool IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, float Tolerance = 0.08f) const;
	/** フィルタ色との一致確認（許容誤差あり） */
	bool IsColorMatch(const FLinearColor& FilterColor, float Tolerance = 0.08f) const;
	/** マッチ結果を使った色一致確認 */
	bool CheckColorMatch(FEffectMatchResult MatchResult, const FLinearColor& FilterColor, bool bUseComplementary = false) const;
	/** 色が変更可能かを取得 */
	bool IsChangeable() const;
	/** 非表示状態かを取得 */
	bool IsHidden() const;

	// ====== Getter ======
	/** 色が外部から変更可能かを取得 */
	bool IsColorModifiable() const { return bSetColor; }
	/** 現在の色を取得 */
	FLinearColor GetCurrentColor() const { return CurrentColor; }
	/** 登録されているイベントIDを取得 */
	FName GetColorEventID() const { return ColorEventID; }
	/** このターゲットのタイプを取得 */
	EColorTargetType GetColorTargetType() const { return TargetType; }

private:
	// ====== 共通補助関数 ======
	/** スタティックメッシュを取得 */
	USkeletalMeshComponent* GetStaticMesh() const;
	/** レベルマネージャを取得 */
	ALevelManager* GetLevelManager() const;
	/** カラーマネージャを取得 */
	UColorManager* GetColorManager() const;

protected:
	// ====== コンポーネント参照 ======
	/** ビート同期処理用コンポーネント */
	UPROPERTY()
	UBeatScalerComponent* BeatScaler;

	/** 色リアクティブ処理のクラス */
	UPROPERTY(EditAnywhere, Category = "Reactive")
	TSubclassOf<UColorReactiveComponent> ReactiveComponentClass;

	/** 色リアクティブ処理インスタンス */
	UPROPERTY()
	UColorReactiveComponent* ColorReactive;

	// ====== 色データ ======
	/** 初期色 */
	UPROPERTY(EditAnywhere, Category = "Color")
	FLinearColor StartColor;

	/** 現在の色 */
	UPROPERTY(VisibleAnywhere, Category = "Color")
	FLinearColor CurrentColor;

	// ====== 設定値 ======
	/** このターゲットのタイプ */
	UPROPERTY(EditAnywhere, Category = "Color")
	EColorTargetType TargetType;

	/** 色イベントの識別ID */
	UPROPERTY(EditAnywhere, Category = "Color")
	FName ColorEventID;

	/** 色変数として扱うか（ロック切替用） */
	UPROPERTY(EditAnywhere, Category = "Color")
	bool bIsColorVariable = false;

	/** 色を変更可能か */
	UPROPERTY(EditAnywhere, Category = "Color")
	bool bColorChangeable = true;

	/** 初期設定時に色を適用するか */
	UPROPERTY(EditAnywhere, Category = "Color")
	bool bSetColor = true;

	/** 色に応じたアクションを有効にするか */
	UPROPERTY(EditAnywhere, Category = "Color")
	bool bEnableColorAction = true;

	/** 補色を使用するか */
	UPROPERTY(EditAnywhere, Category = "Color")
	bool bUseComplementaryColor = false;

	/** 現在の色が一致しているか */
	UPROPERTY(VisibleAnywhere, Category = "Color")
	bool bIsColorMatch = false;

	/** このオブジェクトに適用されるエフェクト */
	UPROPERTY(EditAnywhere, Category = "Color")
	EBuffEffect EffectType;

	/** 関連するナイアガラアクター */
	UPROPERTY(EditAnywhere, Category = "Effect")
	TArray<ANiagaraActor*> NiagaraActors;

private:
	/** 選択中かどうか */
	bool bIsSelected = false;

	/** ビート演出を再生するか */
	UPROPERTY(EditAnywhere, Category = "Beat")
	bool bPlayBeatEffect = true;
};
