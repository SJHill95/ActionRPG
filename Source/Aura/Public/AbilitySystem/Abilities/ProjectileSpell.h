// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MainGameplayAbility.h"
#include "ProjectileSpell.generated.h"

class ABaseProjectile;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class AURA_API UProjectileSpell : public UMainGameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

};
