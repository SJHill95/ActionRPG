// Copyright Sam Hill


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/MainGameInstance.h"
#include "Game/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName("LoadSlot_0");
	LoadSlot_0->SetSlotIndex(0);
	LoadSlots.Add(0, LoadSlot_0);
	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName("LoadSlot_1");
	LoadSlot_0->SetSlotIndex(1);
	LoadSlots.Add(1, LoadSlot_1);
	
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName("LoadSlot_2");
	LoadSlot_0->SetSlotIndex(2);
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	AMainGameModeBase* MainGameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));

	LoadSlots[Slot]->SetMapName(MainGameMode->DefaultMapName);
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SlotStatus = Taken;

	MainGameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();

	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(this));
	MainGameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();
	MainGameInstance->LoadSlotIndex = LoadSlots[Slot]->GetSlotIndex();
	MainGameInstance->PlayerStartTag = MainGameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AMainGameModeBase::DeleteSlot(SelectedSlot, SelectedSlot->GetSlotIndex());
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
	
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (AMainGameModeBase* MainGameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (IsValid(SelectedSlot))
		{
			MainGameMode->TravelToMap(SelectedSlot);
		}
	}
}

void UMVVM_LoadScreen::LoadData()
{
	AMainGameModeBase* MainGameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveObject = MainGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);

		LoadSlot.Value->SlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(SaveObject->PlayerName);
		LoadSlot.Value->InitializeSlot();
		LoadSlot.Value->SetMapName(SaveObject->MapName);
	}
}
