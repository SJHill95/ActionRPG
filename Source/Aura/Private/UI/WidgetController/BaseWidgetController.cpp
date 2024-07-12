// Copyright Sam Hill


#include "UI/WidgetController/BaseWidgetController.h"

#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "AbilitySystem/BaseAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/MainPlayerController.h"
#include "Player/MainPlayerState.h"

void UBaseWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UBaseWidgetController::BroadcastInitialValues()
{
	
}

void UBaseWidgetController::BindCallbacksToDependencies()
{
	
}

void UBaseWidgetController::BroadcastAbilityInfo()
{
	if (!GetBaseASC()->bStartupAbilitiesGiven)
	{
		return;
	}

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FMainAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(BaseAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = BaseAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = BaseAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
		
	});
	GetBaseASC()->ForEachAbility(BroadcastDelegate);
}

AMainPlayerController* UBaseWidgetController::GetMainPC()
{
	if (MainPlayerController == nullptr)
	{
		MainPlayerController = Cast<AMainPlayerController>(PlayerController);
	}
	return MainPlayerController;
}

AMainPlayerState* UBaseWidgetController::GetMainPS()
{
	if (MainPlayerState == nullptr)
	{
		MainPlayerState = Cast<AMainPlayerState>(PlayerState);
	}
	return MainPlayerState;
}

UBaseAbilitySystemComponent* UBaseWidgetController::GetBaseASC()
{
	if (BaseAbilitySystemComponent == nullptr)
	{
		BaseAbilitySystemComponent = Cast<UBaseAbilitySystemComponent>(AbilitySystemComponent);
	}
	return BaseAbilitySystemComponent;
}

UBaseAttributeSet* UBaseWidgetController::GetBaseAS()
{
	if (BaseAttributeSet == nullptr)
	{
		BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
	}
	return BaseAttributeSet;
}
