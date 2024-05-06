// Copyright Sam Hill


#include "UI/Widget/BaseUserWidget.h"

#include "UnrealWidgetFwd.h"

void UBaseUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
