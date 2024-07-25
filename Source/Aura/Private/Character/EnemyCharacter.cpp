// Copyright Sam Hill


#include "Character/EnemyCharacter.h"

#include "MainGameplayTags.h"
#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "AbilitySystem/BaseAttributeSet.h"
#include "AbilitySystem/MainAbilitySystemLibrary.h"
#include "AI/MainAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/BaseUserWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBarWidget");
	HealthBar->SetupAttachment(GetRootComponent());

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}
	MainAIController = Cast<AMainAIController>(NewController);

	MainAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	MainAIController->RunBehaviorTree(BehaviorTree);
	MainAIController->GetBlackboardComponent()->SetValueAsBool(FName("bHitReacting"), false);
	MainAIController->GetBlackboardComponent()->SetValueAsBool(FName("bRangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	
}

void AEnemyCharacter::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

int32 AEnemyCharacter::GetPlayerLevel_Implementation()
{
	return Level;
}

void AEnemyCharacter::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (MainAIController != nullptr)
	{
		MainAIController->GetBlackboardComponent()->SetValueAsBool(FName("bDead"), true);
	}
	HealthBar->SetVisibility(false);
	Super::Die(DeathImpulse);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	InitAbilityActorInfo();

	if (HasAuthority())
	{
		UMainAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (UBaseUserWidget* BaseUserWidget = Cast<UBaseUserWidget>( HealthBar->GetUserWidgetObject()))
	{
		BaseUserWidget->SetWidgetController(this);
	}
	
	if (const UBaseAttributeSet* BaseAS = Cast<UBaseAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast((Data.NewValue));
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast((Data.NewValue));
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FMainGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AEnemyCharacter::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(BaseAS->GetHealth());
		OnMaxHealthChanged.Broadcast(BaseAS->GetMaxHealth());
	}
	
}

void AEnemyCharacter::HitReactTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (MainAIController && MainAIController->GetBlackboardComponent())
	{
		MainAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AEnemyCharacter::InitAbilityActorInfo()
{
	check (AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UBaseAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}

	OnAscRegistered.Broadcast(AbilitySystemComponent);
}

void AEnemyCharacter::InitializeDefaultAttributes() const
{
	UMainAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
