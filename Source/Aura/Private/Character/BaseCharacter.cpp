// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "MainGameplayTags.h"
#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	const FMainGameplayTags& GameplayTags = FMainGameplayTags::Get();

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = GameplayTags.Debuff_Burn;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseCharacter::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

UNiagaraSystem* ABaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ABaseCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if(TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 ABaseCharacter::GetMinionCount_Implementation()
{
	return MinionCount;
}

void ABaseCharacter::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass ABaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered ABaseCharacter::GetOnAscRegisteredDelegate()
{
	return OnAscRegistered;
}

FOnDeathDelegate ABaseCharacter::GetOnDeathDelegate()
{
	return OnDeath;
}

void ABaseCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
	
	OnDeath.Broadcast(this);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::InitAbilityActorInfo()
{
}

FVector ABaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FMainGameplayTags& GameplayTags = FMainGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}

	return FVector();
	
}

UAnimMontage* ABaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ABaseCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ABaseCharacter::AddCharacterAbilities()
{
	UBaseAbilitySystemComponent* MainASC = CastChecked<UBaseAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	MainASC->AddCharacterAbilities(StartupAbilities);
	MainASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
	
}

void ABaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}
