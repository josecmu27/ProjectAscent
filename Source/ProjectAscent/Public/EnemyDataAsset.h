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
	/* Combat */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DetectionSightRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float LoseSightRadius; // should be higher than detection radius

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AlignmentThreshold;

	/* Movement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float PatrolSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float InvestigationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RotationSpeed;

	/* Attack */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float ThrowableCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float ThrowableThrowRange;

	/* General */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General")
	float AlertDuration; // Time Enemy stays alerted when player is spotted

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General")
	TArray<TSubclassOf<AInteractableActor>> DropItems; // Array of items dropped when enemy gets eliminated
	
};
