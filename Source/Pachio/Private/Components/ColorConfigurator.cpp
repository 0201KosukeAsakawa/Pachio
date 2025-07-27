#include "Components/ColorConfigurator.h"
#include "Components/ColorReactiveComponent.h"
#include "Components/BeatScalerComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"

// �R���X�g���N�^�FTick �̓f�t�H���g�Ŗ����i��{�I�Ƀ��A���^�C���X�V�s�v�j
UColorConfigurator::UColorConfigurator()
{
	BeatScalerComponent = CreateDefaultSubobject<UBeatScalerComponent>(TEXT("BeatScalerComponent"));
}

// �F�����I�u�W�F�N�g�̏���������
void UColorConfigurator::Init()
{
	InitializeColorLogic();    // �F�����R���|�[�l���g�̐����E�ݒ�
	RegisterToColorManager(); // �J���[�}�l�[�W���[�ւ̓o�^
	SetupMaterial();          // �}�e���A���ƃX�e���V���l�̐ݒ�
	// ���������iBeginPlay�Ȃǁj
	if (USoundManager* soundManager = Cast<USoundManager>(ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetObject()))
	{
		soundManager->OnBeatDetected.AddDynamic(this, &UColorConfigurator::PlayBeatAnimation);
	}
}

// �F�������W�b�N�̏������iUColorReactiveComponent�̐����j
void UColorConfigurator::InitializeColorLogic()
{
	if (ReactiveComponentClass == nullptr)
		return;
	CurrentColor = StartColor;
	// �w�肳�ꂽ�N���X����C���X�^���X�𐶐�
	ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (ColorReactiveComponent == nullptr)
		return;

	// �R���|�[�l���g�̓o�^�ƃA�N�e�B�x�[�g
	ColorReactiveComponent->RegisterComponent();
	ColorReactiveComponent->Activate(true);
	ColorReactiveComponent->SetMyColor(StartColor);

	// StaticMesh �Ƀo�C���h�i�F�̔����Ώۃ��b�V���擾�j
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	ColorReactiveComponent->Init(Mesh); // ���b�V����o�^���ď�����
}

// ���x����̃J���[�}�l�[�W���[�Ɏ��g��o�^
void UColorConfigurator::RegisterToColorManager()
{
	ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
	if (LevelManager == nullptr)
		return;

	UColorManager* ColorManager = LevelManager->GetColorManager();
	if (ColorManager == nullptr)
		return;

	// �^�[�Q�b�g��ʂƂƂ�Ɏ�����o�^
	ColorManager->RegisterTarget(ColorTargetType, this);
}

// �}�e���A���ƃJ�X�^���f�v�X�ݒ�
void UColorConfigurator::SetupMaterial()
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	// �֊s�`��p�̃f�v�X�X�e���V���ݒ�
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(10);

	// �_�C�i�~�b�N�}�e���A���쐬�ƃx�[�X�F�̐ݒ�
	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial == nullptr || !bSetColor)
		return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), StartColor);
}

void UColorConfigurator::PlayBeatAnimation()
{
	if (BeatScalerComponent)
		BeatScalerComponent->PlayBeat();
}

// �F�A�N�V�������s���̏����i�f�t�H���g�����j
void UColorConfigurator::ColorAction(FLinearColor NewColor)
{
	if (!bPlayColorAction || ColorReactiveComponent == nullptr)
		return;
	if (bColorVariable)
		ApplyColorToMaterial(NewColor);

	// ���͐F�ƈ�v���邩�`�F�b�N�i���ʂ� bColorMuch �ɕێ��j
	bColorMuch = ColorReactiveComponent->CheckColorMatch(NewColor, buseComplementaryColor);
}

void UColorConfigurator::SetColor(FLinearColor newColor)
{
	CurrentColor = newColor;
	ApplyColorToMaterial(CurrentColor);
	ColorReactiveComponent->SetMyColor(CurrentColor);
	ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
	if (LevelManager == nullptr)
		return;

	UColorManager* ColorManager = LevelManager->GetColorManager();
	if (ColorManager == nullptr)
		return;
	ColorAction(ColorManager->GetWorldColor());
}

void UColorConfigurator::ResetColor()
{
	SetColor(StartColor);
}

void UColorConfigurator::SetColorMuch(bool b)
{
	bColorMuch = b;
}

bool UColorConfigurator::IsColorChange() const
{
	if (!ColorReactiveComponent)
		return false;

	return ColorReactiveComponent->IsColorMatch(StartColor);
}

bool UColorConfigurator::IsColorChange(FLinearColor color) const
{
	if (!ColorReactiveComponent)
		return false;

	return ColorReactiveComponent->IsColorMatch(color);
}

bool UColorConfigurator::CheckColorMatch(const FLinearColor& FilterColor, bool ComplementaryColor) const
{
	if (!ColorReactiveComponent)
		return false;
	return ColorReactiveComponent->CheckColorMatch(FilterColor, ComplementaryColor);
}

bool UColorConfigurator::IsColorMatch() const
{
	return bColorMuch;
}

bool UColorConfigurator::IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance)const
{
	if (!ColorReactiveComponent)
		return false;
	return ColorReactiveComponent->IsColorMatch(FilterColor, TargetColor, Tolerance);
}

bool UColorConfigurator::IsColorMatch(const FLinearColor& FilterColor, const float Tolerance) const
{
	if (!ColorReactiveComponent)
		return false;
	return ColorReactiveComponent->IsColorMatch(FilterColor, Tolerance);
}

// �}�e���A���ɐF��K�p�i�O������蓮�K�p����p�j
void UColorConfigurator::ApplyColorToMaterial(FLinearColor InColor)
{
	ColorReactiveComponent->ApplyColorToMaterial(InColor);
}
