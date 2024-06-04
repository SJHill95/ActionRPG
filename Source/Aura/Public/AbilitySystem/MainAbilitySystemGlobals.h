// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "MainAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMainAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
