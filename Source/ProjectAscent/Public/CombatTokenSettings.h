// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CombatTokenSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Combat Token Settings"))
class PROJECTASCENT_API UCombatTokenSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	

public:
    UPROPERTY(EditAnywhere, config, Category = "Tuning")
    float StealCooldownDuration = 0.0f;

    UPROPERTY(EditAnywhere, config, Category = "Tuning")
    float MinStealMargin = 0.0f;

};
