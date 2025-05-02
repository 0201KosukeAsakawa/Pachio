#include "Objects/Goal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h" // これを追加

// Sets default values
AGoal::AGoal()
{
	// Initialize default values
	PrimaryActorTick.bCanEverTick = true;
	isGoal = false;

	// ゴールボックスコンポーネントを作成
	GoalBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalBox"));
	RootComponent = GoalBox;

	// コリジョンの初期設定
	GoalBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // クエリのみ
	GoalBox->SetCollisionResponseToAllChannels(ECR_Ignore); // 他のすべてのコリジョンを無視
	GoalBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // プレイヤーとの衝突をオーバーラップに設定

	// 衝突開始時に呼ばれるイベントをバインド
	GoalBox->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnGoalOverlap);
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(isGoal)
	UGameplayStatics::OpenLevel(this,nextWorldName);
}

// 衝突時に呼ばれる関数
void AGoal::OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	// プレイヤー（Pawn）がゴールに触れた場合
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		// ゴール達成フラグをオンにする
		isGoal = true;
		UE_LOG(LogTemp, Warning, TEXT("ゴール達成！"));
	}
}