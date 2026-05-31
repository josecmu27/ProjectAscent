// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectAscentHUD.h"
#include "Blueprint/UserWidget.h"

void AProjectAscentHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(HealthBarWidgetClass) || !IsValid(CrosshairWidgetClass))
	{
		return;
	}

	HealthBarWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), HealthBarWidgetClass);
	CrosshairWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), CrosshairWidgetClass);

	if (!IsValid(HealthBarWidget) || !IsValid(CrosshairWidget))
	{
		return;
	}

	HealthBarWidget->AddToViewport();
	CrosshairWidget->AddToViewport();
}