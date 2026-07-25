// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RequestCombatToken.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API UBTService_RequestCombatToken : public UBTService
{
	GENERATED_BODY()

    UBTService_RequestCombatToken();

protected:
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
