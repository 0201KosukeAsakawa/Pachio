#include "Enemy/EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Enemy/State/EnemyStateComponent.h"


AEnemyCharacter::AEnemyCharacter()
{
	// 毎フレーム Tick を呼び出す設定
	PrimaryActorTick.bCanEverTick = true;

	// コリジョン（当たり判定）用のボックスコンポーネントをルートとして設定
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComponent"));
	RootComponent = BoxComponent;

	// 見た目用のメッシュコンポーネントを作成し、ボックスにアタッチ
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	Logic = NewObject<UEnemyStateComponent>(this);

	if (!Logic)
		return;

	Logic->OnEnter(this);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Logic)
		return;

	Logic->OnUpdate(DeltaTime);
}