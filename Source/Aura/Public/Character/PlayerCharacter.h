// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UGameplayEffect;
class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class AURA_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter();
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	/** Combat Interface */
	virtual int32 GetPlayerLevel() override;

	/** end Combat Interface */

private:
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* Camera;
	
};
