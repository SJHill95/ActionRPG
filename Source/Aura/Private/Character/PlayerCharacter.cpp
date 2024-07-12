// Copyright Sam Hill


#include "Character/PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/MainPlayerController.h"
#include "Player/MainPlayerState.h"
#include "UI/HUD/MainHUD.h"

APlayerCharacter::APlayerCharacter()
{
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	SpringArm->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void APlayerCharacter::InitAbilityActorInfo()
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check (MainPlayerState);
	MainPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MainPlayerState, this);
	Cast<UBaseAbilitySystemComponent>(MainPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = MainPlayerState->GetAbilitySystemComponent();
	AttributeSet = MainPlayerState->GetAttributeSet();

	 if (AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(GetController()))
	 {
	 	if (AMainHUD* MainHUD = Cast<AMainHUD>(MainPlayerController->GetHUD()))
	 	{
	 		MainHUD->InitOverlay(MainPlayerController, MainPlayerState, AbilitySystemComponent, AttributeSet);
	 	}
	 }

	InitializeDefaultAttributes();
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
	
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the client
	InitAbilityActorInfo();
	
}

void APlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToXP(InXP);
}

void APlayerCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void APlayerCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = Camera->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		
		LevelUpNiagaraComponent->Activate(true);
	}
}


int32 APlayerCharacter::GetXP_Implementation() const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetXP();
}

int32 APlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 APlayerCharacter::GetAttributePointReward_Implementation(int32 Level) const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointReward;
}

int32 APlayerCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointReward;
}

void APlayerCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToLevel(InPlayerLevel);

	if (UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(AbilitySystemComponent))
	{
		BaseASC->UpdateAbilityStatuses(MainPlayerState->GetPlayerLevel());
	}
	
}

void APlayerCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToAttributePoints(InAttributePoints);
}

void APlayerCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	MainPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 APlayerCharacter::GetAttributePoints_Implementation() const
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetAttributePoints();
}

int32 APlayerCharacter::GetSpellPoints_Implementation() const
{
	AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetSpellPoints();
}

int32 APlayerCharacter::GetPlayerLevel_Implementation()
{
	const AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
	check(MainPlayerState);
	return MainPlayerState->GetPlayerLevel();
}
