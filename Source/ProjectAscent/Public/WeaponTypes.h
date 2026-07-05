// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None		UMETA(DisplayName = "None"),
	Rifle		UMETA(DisplayName = "Rifle"),
	Shotgun		UMETA(DisplayName = "Shotgun")
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	SlotOne,
	SlotTwo
};

class PROJECTASCENT_API WeaponTypes
{
public:
	WeaponTypes();

};
