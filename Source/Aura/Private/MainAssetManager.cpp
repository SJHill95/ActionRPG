// Copyright Sam Hill


#include "MainAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "MainGameplayTags.h"

UMainAssetManager& UMainAssetManager::Get()
{
	check (GEngine);
	
	UMainAssetManager* MainAssetManager = Cast<UMainAssetManager>(GEngine->AssetManager);
	return *MainAssetManager;
}

void UMainAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FMainGameplayTags::InitializeNativeGameplayTags();
	
	// This is required to use Target Data!
	UAbilitySystemGlobals::Get().InitGlobalData();
}
