// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectAscentGameMode.h"
#include "ProjectAscentCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectAscentHUD.h"

AProjectAscentGameMode::AProjectAscentGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ProjectAscentHUD"));

	if (HUDBPClass.Class != NULL)
	{
		HUDClass = HUDBPClass.Class;
	}

}