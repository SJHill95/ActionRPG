// Copyright Sam Hill


#include "AbilitySystem/MainAbilitySystemGlobals.h"
#include "MainAbilityTypes.h"

FGameplayEffectContext* UMainAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMainGameplayEffectContext();
}
