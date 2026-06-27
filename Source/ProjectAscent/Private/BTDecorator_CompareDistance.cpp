// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CompareDistance.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CompareDistance::UBTDecorator_CompareDistance()
{
	OriginActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CompareDistance, OriginActorKey), AActor::StaticClass());
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CompareDistance, TargetActorKey), AActor::StaticClass());

}

/**
 * @brief determines if TargetActor is within max distance of Origin Actor
 *
 * @param[in] OwnerComp Behavior Tree component running this task
 * @param[in] NodeMemory Per-instance memory block (unused in this task)
 */
bool UBTDecorator_CompareDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
    if (!IsValid(BBComponent)) return false;

    AActor* OriginActor = Cast<AActor>(BBComponent->GetValueAsObject(OriginActorKey.SelectedKeyName));
    AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));

    if (!IsValid(OriginActor) || !IsValid(TargetActor)) return false;

    float Distance = FVector::Dist(OriginActor->GetActorLocation(), TargetActor->GetActorLocation());

    return Distance < MaxDistance;
}
