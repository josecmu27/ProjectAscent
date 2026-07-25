// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatRoleSubsystem.generated.h"


UENUM(BlueprintType)
enum class ECombatRole : uint8
{
	Engager	   UMETA(DisplayName = "Engager"),
	Defender   UMETA(DisplayName = "Defender")
};

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API UCombatRoleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

private:

};
