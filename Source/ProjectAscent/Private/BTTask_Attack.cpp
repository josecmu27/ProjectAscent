// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "EnemyBase.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    if (!IsValid(Enemy))
    {
        return EBTNodeResult::Failed;
    }

    Enemy->Attack();

    return EBTNodeResult::Succeeded;
}