// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_RequestCombatToken.h"
#include "CombatTokenSubsystem.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"  
#include "EnemyBase.h"

UBTService_RequestCombatToken::UBTService_RequestCombatToken()
{

    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
    bNotifyTick = false;
}

void UBTService_RequestCombatToken::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard || Blackboard->GetValueAsBool("bHasCombatToken")) return;


    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr);
    if (!Enemy) return;



    if (UCombatTokenSubsystem* TokenSubsystem = Enemy->GetWorld()->GetSubsystem<UCombatTokenSubsystem>())
    {
        TokenSubsystem->RequestToken(Enemy);
    
    }
}

void UBTService_RequestCombatToken::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard || Blackboard->GetValueAsBool("bHasCombatToken")) return;


    AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr);
    if (!Enemy) return;


    if (UCombatTokenSubsystem* TokenSubsystem = Enemy->GetWorld()->GetSubsystem<UCombatTokenSubsystem>())
    {
        TokenSubsystem->ReturnToken(Enemy);

    }
}
