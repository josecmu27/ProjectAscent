// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GrapplingHookDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API UGrapplingHookDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/* Player Movement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement")
	float MinRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement")
	float FlyingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement")
	float FlyingInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement")
	float FiringInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement")
	float TargetDistanceThreshold;

	/* Prop Grappling */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop Grappling")
	float PropPullSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop Grappling")
	float PropTargetOffset; // Distance at which prop grappling should stop 

	/* Enemy Grappling*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Grappling")
	float EnemyPullForce;

};
