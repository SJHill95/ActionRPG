// Copyright Sam Hill


#include "UI/HUD/MainHUD.h"

#include "UI/Widget/BaseUserWidget.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	OverlayWidget = CreateWidget<UBaseUserWidget>(GetWorld(), OverlayWidgetClass);

	OverlayWidget->AddToViewport();
}
