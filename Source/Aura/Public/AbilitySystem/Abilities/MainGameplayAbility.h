// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MainGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMainGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;
	
};
