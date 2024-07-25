// Copyright Sam Hill


#include "AbilitySystem/Abilities/MainDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UMainDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle =  MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,DamageType, ScaledDamage);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

FDamageEffectParams UMainDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	if (IsValid(TargetActor))
	{
		const bool bKnockback = FMath::RandRange(1, 100) < KnockbackChance;
		if (bKnockback)
		{
			FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
			Rotation.Pitch = 45.f;
			const FVector ToTarget = Rotation.Vector();
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
		
	}
	return Params;
}

FTaggedMontage UMainDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		return TaggedMontages[FMath::RandRange(0, TaggedMontages.Num() -1)];
	}
	return FTaggedMontage();
	
}
