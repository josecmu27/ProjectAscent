// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AimAtTarget.h"
#include "AIController.h"
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

void UBTTask_AimAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{  
    UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
    if (!IsValid(BBComponent)) return;

    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
    AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));

    if (!IsValid(Enemy) || !IsValid(TargetActor)) return;
    
    // Aim at player
    FRotator CurrentRotation = Enemy->GetActorRotation();
    FRotator TargetRotation = (TargetActor->GetActorLocation() - Enemy->GetActorLocation()).Rotation();
    Enemy->SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, Enemy->GetDataAsset()->RotationSpeed));

    // Check if aim direction is closely alligned to the player 
    FVector CurrentForward = Enemy->GetActorForwardVector();
    FVector DesiredForward = (TargetActor->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
    
    float DotProduct = FVector::DotProduct(CurrentForward, DesiredForward);
    float ThresholdDot = FMath::Cos(FMath::DegreesToRadians(Enemy->GetDataAsset()->AlignmentThreshold));

    if (DotProduct >= ThresholdDot)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }


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


