// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * @brief Sets the enemy's MaxWalkSpeed Enemy depending of the selected speed type in
 *        the behavior tree editor. Reads the actual speed value from the enemy's 
 *        EnemyDataAsset to prevent duplicating speed values in multiple places
 *
 * @param[in] OwnerComp Behavior Tree component running this task
 * @param[in] NodeMemory Per-instance memory block (unused in this task)
 */
EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    if (!IsValid(Enemy) || !IsValid(Enemy->GetDataAsset()))
    {
        return EBTNodeResult::Failed;
    }

    float Speed = 0.0f;

    // Set speed depending on speed type
    switch (SpeedType)
    {
    case EMovementSpeedType::Patrol:
        Speed = Enemy->GetDataAsset()->PatrolSpeed;
        break;
    case EMovementSpeedType::Chase:
        Speed = Enemy->GetDataAsset()->ChaseSpeed;
        break;
    case EMovementSpeedType::Strafe:
        Speed = Enemy->GetDataAsset()->StrafeSpeed;
        break;
    case EMovementSpeedType::Investigate:
        Speed = Enemy->GetDataAsset()->InvestigateSpeed;
        break;
    }

    Enemy->GetCharacterMovement()->MaxWalkSpeed = Speed;

    return EBTNodeResult::Succeeded;
}