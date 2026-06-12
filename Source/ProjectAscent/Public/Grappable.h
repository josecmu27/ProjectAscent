// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrapplingHookDataAsset.h"
#include "Grappable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGrappable : public UInterface
{
	GENERATED_BODY()
};


class PROJECTASCENT_API IGrappable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grapple")
	void GrapplePull(float Magnitude, FVector PlayerLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grapple")
	void GrapplePullCancel();
};
