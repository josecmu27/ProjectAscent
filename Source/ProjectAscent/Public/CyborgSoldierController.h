// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "CyborgSoldierController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API ACyborgSoldierController : public AEnemyAIController
{
	GENERATED_BODY()

protected:
    virtual void HandlePlayerLost(FVector LastLocation) override;

    virtual void HandlePlayerSeen(AActor* Player) override;

private:
    void AlertAllSoldiers(AActor* Player);

    void TellNearbySoldiersToInvestigate(FVector LastLocation);

};
