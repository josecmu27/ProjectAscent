// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CompareDistance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API UBTDecorator_CompareDistance : public UBTDecorator
{
	GENERATED_BODY()
	
public:

	UBTDecorator_CompareDistance();

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector OriginActorKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxDistance;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


};
