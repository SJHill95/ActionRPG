// Copyright Sam Hill


#include "Actor/FireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MainAbilitySystemGlobals.h"
#include "AbilitySystem/MainAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "GameplayCueManager.h"

void AFireBall::BeginPlay()
{
	Super::BeginPlay();

	StartOutgoingTimeline();
	
}

void AFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UMainAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AFireBall::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FMainGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}
	
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	
	bHit = true;
}
