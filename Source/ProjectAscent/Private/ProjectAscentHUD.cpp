// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectAscentHUD.h"
#include "Blueprint/UserWidget.h"

void AProjectAscentHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(HealthBarWidgetClass))
	{
		return;
	}

	HealthBarWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), HealthBarWidgetClass);

	if (!IsValid(HealthBarWidget))
	{
		return;
	}

	HealthBarWidget->AddToViewport();
}