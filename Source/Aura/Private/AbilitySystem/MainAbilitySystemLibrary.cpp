// Copyright Sam Hill


#include "AbilitySystem/MainAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MainAbilityTypes.h"
#include "Game/MainGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainPlayerState.h"
#include "UI/HUD/MainHUD.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* UMainAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AMainHUD* MainHUD = Cast<AMainHUD>(PC->GetHUD()))
		{
			AMainPlayerState* PS = PC->GetPlayerState<AMainPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MainHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UMainAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AMainHUD* MainHUD = Cast<AMainHUD>(PC->GetHUD()))
		{
			AMainPlayerState* PS = PC->GetPlayerState<AMainPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MainHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

USpellMenuWidgetController* UMainAbilitySystemLibrary::GetSpellMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AMainHUD* MainHUD = Cast<AMainHUD>(PC->GetHUD()))
		{
			AMainPlayerState* PS = PC->GetPlayerState<AMainPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MainHUD->GetSpellMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UMainAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	
}

void UMainAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	
	if (CharacterClassInfo == nullptr)
	{
		return;
	}
	
	for(TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}


int32 UMainAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	
	if (CharacterClassInfo == nullptr)
	{
		return 0;
	}

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

FGameplayEffectContextHandle UMainAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FMainGameplayTags& GameplayTags = FMainGameplayTags::Get();
	
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	
	DamageEffectParams.TargetAbilitySystemComponent.Get()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	return EffectContextHandle;
}

UCharacterClassInfo* UMainAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AMainGameModeBase* MainGameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (MainGameMode == nullptr)
	{
		return nullptr;
	}

	return MainGameMode->CharacterClassInfo;
}

UAbilityInfo* UMainAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AMainGameModeBase* MainGameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (MainGameMode == nullptr)
	{
		return nullptr;
	}

	return MainGameMode->AbilityInfo;
}

bool UMainAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MainEffectContext->IsBlockedHit();
	}
	return false;
	
}

bool UMainAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MainEffectContext->IsCriticalHit();
	}
	return false;
}

bool UMainAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MainEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UMainAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MainEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UMainAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MainEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UMainAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MainEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UMainAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMainGameplayEffectContext* MainEffectContext = static_cast<const FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (MainEffectContext->GetDamageType().IsValid())
		{
			return *MainEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

void UMainAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MainEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UMainAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MainEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UMainAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsSuccessfulDebuff)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MainEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UMainAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MainEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UMainAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDuration)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MainEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UMainAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffFrequency)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MainEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

inline void UMainAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FMainGameplayEffectContext* MainEffectContext = static_cast<FMainGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		MainEffectContext->SetDamageType(DamageType);
	}
}

void UMainAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                           TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                           const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UMainAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies= FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;

	
}
