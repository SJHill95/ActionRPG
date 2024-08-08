// Copyright Sam Hill

#pragma once

#include "CoreMinimal.h"
#include "Actor/BaseProjectile.h"
#include "FireBall.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AFireBall : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	
	FORCEINLINE void SetReturnActor(AActor* InReturnActor) { ReturnActor = InReturnActor; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetReturnActor() const { return ReturnActor; }
	
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;

protected:
	virtual void BeginPlay() override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnActor;

	virtual void OnHit() override;

};
