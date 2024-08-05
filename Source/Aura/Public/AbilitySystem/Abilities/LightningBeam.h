// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BeamSpell.h"
#include "LightningBeam.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ULightningBeam : public UBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
};
