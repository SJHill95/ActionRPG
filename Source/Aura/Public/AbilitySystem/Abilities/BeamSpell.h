// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MainDamageGameplayAbility.h"
#include "BeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBeamSpell : public UMainDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam")
	FName SocketName = ("TipSocket");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Beam")
	float ChainLightningEffectRadius = 850.f;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxNumShockTargets = 5;
};