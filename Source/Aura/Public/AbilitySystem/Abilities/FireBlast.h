// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MainDamageGameplayAbility.h"
#include "FireBlast.generated.h"

class AFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UFireBlast : public UMainDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AFireBall*> SpawnFireBalls();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireBlast")
	int32 NumFireBalls = 12;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFireBall> FireBallClass;
	
};
