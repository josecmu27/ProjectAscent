// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "ProjectAscentHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API AProjectAscentHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category="HealthBar")
	UUserWidget* HealthBarWidget;

	UPROPERTY(VisibleAnywhere, Category = "HUD")
	UUserWidget* CrosshairWidget;

private:
	UPROPERTY(EditDefaultsOnly, Category="HealthBar")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;


};
