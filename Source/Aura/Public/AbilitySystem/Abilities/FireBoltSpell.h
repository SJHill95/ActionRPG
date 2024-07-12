// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileSpell.h"
#include "FireBoltSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UFireBoltSpell : public UProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
};
