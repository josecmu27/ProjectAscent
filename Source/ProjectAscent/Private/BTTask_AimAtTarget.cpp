// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AimAtTarget.h"
#include "AIController.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyBase.h"

UBTTask_AimAtTarget::UBTTask_AimAtTarget()
{
    bNotifyTick = true;
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AimAtTarget, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_AimAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

    if (IsValid(Enemy))
    {
        Enemy->SetIsAiming(true);
    }

    return EBTNodeResult::InProgress;

}

/*
* @brief Continuously rotates Enemy AI's body (yaw) towards the target actor and updates its
*        AimRotation (used in the Enemy Animation Blueprint and the trace direction for trace attacks). 
*        Runs indefinitely as part of a nested Parallel alongside the Strafe task, it will 
*        only stop via AbortTask when Combat Sequence aborts.
* 
* @param[in] OwnerComp Behavior Tree component running this task
* @param[in] NodeMemory Per-instance memory block (unused in this task)
* @param[in] DeltaSeconds Time since last tick
*/
void UBTTask_AimAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{  
    UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
    if (!IsValid(BBComponent)) return;

    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));

    if (!IsValid(Enemy) || !IsValid(TargetActor) || !IsValid(Enemy->GetDataAsset())) return;

    // Calculate direction and full rotation toward the target
    FVector DirectionToTarget = TargetActor->GetActorLocation() - Enemy->GetActorLocation();
    FRotator TargetRotation = DirectionToTarget.Rotation();
    float RotationSpeed = Enemy->GetDataAsset()->RotationSpeed;

    // Rotate the whole body horizontally (yaw) toward target
    FRotator CurrentBodyRotation = Enemy->GetActorRotation();
    FRotator TargetBodyRotation = FRotator(0.0f, TargetRotation.Yaw, 0.0f);
    FRotator NewBodyRotation = FMath::RInterpTo(CurrentBodyRotation, TargetBodyRotation, DeltaSeconds, RotationSpeed);
    
    Enemy->SetActorRotation(NewBodyRotation);

    // Store full interpolated rotation (pitch + yaw) for Animation Blueprint for spine (upper body)
    // Final full rotation used as trace direction in the Attack task
    Enemy->AimRotation = FMath::RInterpTo(Enemy->AimRotation, TargetRotation, DeltaSeconds, Enemy->GetDataAsset()->RotationSpeed);
}

EBTNodeResult::Type UBTTask_AimAtTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    
    if (IsValid(Enemy))
    {
        Enemy->SetIsAiming(false);
    }

    return EBTNodeResult::Aborted;
}


