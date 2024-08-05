// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

class ULevelUpInfo;
class UAttributeSet;
class UAbilitySystemComponent;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/);

/**
 * 
 */
UCLASS()
class AURA_API AMainPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMainPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

	void SetLevel(const int32 InLevel);

	void AddToLevel(const int32 InLevel);

	FORCEINLINE int32 GetXP() const { return XP; }

	void SetXP(const int32 InXP);

	void AddToXP(const int32 InXP);

	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }

	void SetAttributePoints(const int32 InAttributePoints);
	
	void AddToAttributePoints(const int32 InAttributePoints);
	
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	
	void SetSpellPoints(const int32 InSpellPoints);
	
	void AddToSpellPoints(const int32 InSpellPoints);
	
	FOnPlayerStatChanged OnLevelChangedDelegate;

	FOnPlayerStatChanged OnXPChangedDelegate;

	FOnPlayerStatChanged OnAttributePointsChangedDelegate;

	FOnPlayerStatChanged OnSpellPointsChangedDelegate;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
	
};
