// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectAscentHUD.h"
#include "Blueprint/UserWidget.h"

void AProjectAscentHUD::BeginPlay()
{
	Super::BeginPlay();

    if (IsValid(HUDWidgetClass))
    {
        HUDWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), HUDWidgetClass);
        if (IsValid(HUDWidget))
        {
            HUDWidget->AddToViewport();
        }
    }
}