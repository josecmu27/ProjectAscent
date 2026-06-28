// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InteractableActor.h"
#include "EnemyDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API UEnemyDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/* Perception */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DetectionSightRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float CombatSightRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float LoseSightOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AlertSphereRadius;

	/* Movement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float PatrolSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float InvestigateSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float StrafeSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RotationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float StrafeDirectionChangeInterval;

	/* General */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General")
	float AlertDuration; // Time Enemy stays alerted when player is spotted

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General")
	TArray<TSubclassOf<AInteractableActor>> DropItems; // Array of items dropped when enemy gets eliminated
	
};
