// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Strafe.h"
#include "AIController.h"
#include "EnemyBase.h"

UBTTask_Strafe::UBTTask_Strafe()
{
    bNotifyTick = true;
}

/**
 * @brief Sets the enemy's initial strafe direction
 * @param[in] OwnerComp Behavior Tree component running this task
 * @param[in] NodeMemory Per-instance FBTStrafeMemory memory block 
 */
EBTNodeResult::Type UBTTask_Strafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FBTStrafeMemory* StrafeMemory = reinterpret_cast<FBTStrafeMemory*>(NodeMemory);
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

    if (!StrafeMemory || !IsValid(Enemy) || !IsValid(Enemy->GetDataAsset()))
    {
        return EBTNodeResult::Failed;
    }

    float Direction = FMath::RandBool() ? 1.0f : -1.0f;

    StrafeMemory->StrafeTimer = Enemy->GetDataAsset()->StrafeDirectionChangeInterval;
    StrafeMemory->CurrentStrafeDirection = Direction;

    return EBTNodeResult::InProgress;
}

/**
 * @brief Continuously makes the enemy strafe. Changes the direction of strafe 
 *        when strafe timer reaches zero
 *
 * @param[in] OwnerComp Behavior Tree component running this task
 * @param[in] NodeMemory Per-instance FBTStrafeMemory memory block
 * @param[in] DeltaSeconds Time since last tick
 */
void UBTTask_Strafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

    FBTStrafeMemory* StrafeMemory = reinterpret_cast<FBTStrafeMemory*>(NodeMemory);
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

    if (!StrafeMemory || !IsValid(Enemy) || !IsValid(Enemy->GetDataAsset()))
    {
        return;
    }

    StrafeMemory->StrafeTimer -= DeltaSeconds;

    if (StrafeMemory->StrafeTimer < 0.0f)
    {
        StrafeMemory->StrafeTimer = Enemy->GetDataAsset()->StrafeDirectionChangeInterval;
        StrafeMemory->CurrentStrafeDirection *= -1.0f;
    }

    FVector RightDirection = Enemy->GetActorRightVector() * StrafeMemory->CurrentStrafeDirection;

    // Apply movement to the enemy character
    Enemy->AddMovementInput(RightDirection, 1.0f);
}

uint16 UBTTask_Strafe::GetInstanceMemorySize() const
{
    return sizeof(FBTStrafeMemory);
}