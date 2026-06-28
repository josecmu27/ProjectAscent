// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetMovementSpeed.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMovementSpeedType : uint8
{
    Patrol      UMETA(DisplayName = "Patrol"),
    Investigate UMETA(DisplayName = "Investigate"),
    Chase       UMETA(DisplayName = "Chase"),
    Strafe      UMETA(DisplayName = "Strafe")
};

UCLASS()
class PROJECTASCENT_API UBTTask_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "AI")
    EMovementSpeedType SpeedType;
};
