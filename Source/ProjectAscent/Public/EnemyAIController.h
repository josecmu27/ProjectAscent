// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyBase.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void ExpandSightRadius();

	UFUNCTION()
	void ResetSightRadius();

	/* ------ Blackboard Functions ------ */
	UFUNCTION()
	AActor* GetTargetActor() const;

	UFUNCTION()
	bool GetCanSeePlayer() const;

	UFUNCTION()
	bool GetHasCombatToken() const;

	UFUNCTION()
	void SetCombatToken(bool bHasToken);

protected:

	UFUNCTION()
	virtual void HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	virtual void HandlePlayerSeen(AActor* Player);

	UFUNCTION()
	virtual void HandlePlayerLost(FVector LastLocation);


	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	class UAISenseConfig_Damage* DamageConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
