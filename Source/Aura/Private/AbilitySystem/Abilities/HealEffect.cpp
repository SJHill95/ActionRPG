// Copyright Sam Hill


#include "AbilitySystem/Abilities/HealEffect.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/BaseAttributeSet.h"

UHealEffect::UHealEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo ModifierInfo;
	//ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.f));
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UBaseAttributeSet::GetHealthAttribute();
	//ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FSetByCallerFloat());
	Modifiers.Add(ModifierInfo);
}
