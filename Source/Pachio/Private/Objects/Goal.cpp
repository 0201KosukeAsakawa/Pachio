#include "Objects/Goal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h" // レベル遷移に使用するため追加
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"

// デフォルトコンストラクタ：初期値の設定
AGoal::AGoal()
{
	PrimaryActorTick.bCanEverTick = true; // 毎フレーム Tick を呼ぶ設定
	isGoal = false; // ゴール判定初期化

	// ゴール用の BoxComponent を生成し、ルートとして設定
	GoalBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalBox"));
	RootComponent = GoalBox;

	// コリジョン設定：
	// ・物理処理は行わず、オーバーラップ検知のみ行う
	GoalBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// ・全てのチャンネルに対して無視
	GoalBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	// ・Pawn（プレイヤー）とのオーバーラップのみ有効
	GoalBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// オーバーラップイベント登録：プレイヤーが入ったら OnGoalOverlap を呼ぶ
	GoalBox->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnGoalOverlap);
}

// ゲーム開始時に呼ばれる
void AGoal::BeginPlay()
{
	Super::BeginPlay();
}

// 毎フレーム呼ばれる処理
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ゴールフラグが立っていたら、レベル遷移を実行
	if (isGoal)
	{
		UGameplayStatics::OpenLevel(this, nextWorldName); // nextWorldName に遷移
	}
}

// ゴールにオーバーラップした際のイベント処理
void AGoal::OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ALevelManager* LM = ALevelManager::GetInstance(GetWorld());
		if (LM)
		{
			LM->HandlePlayerGoalReached();
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
		}
	}
}
