// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(ProjectAscenet), meta=(BlueprintSpawnableComponent) )
class PROJECTASCENT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UFUNCTION(BlueprintPure, Category = "Health")
	float const GetCurHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	float const GetMaxHealth();

protected:
	virtual void BeginPlay() override;

private:
	float CurHealth;
	float MaxHealth;
		
};
