// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
    UAIPerceptionComponent* Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*Perception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    GetPerceptionComponent()->ConfigureSense(*SightConfig);
    GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());

    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandlePerceptionUpdated);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
    
    // Set Sight Configuration
    if (IsValid(Enemy) && IsValid(Enemy->GetDataAsset()))
    {
        SightConfig->SightRadius = Enemy->GetDataAsset()->DetectionSightRadius;
        SightConfig->LoseSightRadius = SightConfig->SightRadius + Enemy->GetDataAsset()->LoseSightOffset;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    }

    if (BehaviorTree)
    {
        UBlackboardComponent* BlackboardComponent;
        UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent);
        RunBehaviorTree(BehaviorTree);
    }
}

/**
 * @brief Bound to AIPerceptionComponent::OnTargetPerceptionUpdated. Updates Blackboard
 *        keys when sight of the player is gained or lost, and triggers side effects such as
 *        expanding/reseting combat sight radius, and calling the virtual HandlePlayerLost hook
 * 
 * @param[in] Actor The actor whose perception status changed
 * @param[in] Stimulus Perception data for this update
 */
void AEnemyAIController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    if (Actor != PlayerPawn) return;

    UBlackboardComponent* BBComponent = GetBlackboardComponent();
    
    if (!IsValid(BBComponent)) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        BBComponent->SetValueAsBool(FName("bCanSeePlayer"), true);
        BBComponent->SetValueAsObject(FName("TargetActor"), Actor);

        ExpandSightRadius();
        HandlePlayerSeen(Actor);
    }
    else
    {
        BBComponent->SetValueAsBool(FName("bCanSeePlayer"), false);
        BBComponent->SetValueAsVector(FName("LastKnownPlayerLocation"), Actor->GetActorLocation());
        
        ResetSightRadius();
        HandlePlayerLost(Actor->GetActorLocation());
    }
}

/**
 * @brief Virtual hook called when sight of the player is seen. Override for 
 *        specific enemy reactions.
 *
 * @param[in] LastLocation The last location where the player has been seen
 */
void AEnemyAIController::HandlePlayerSeen(AActor* Player)
{

}


/**
 * @brief Virtual hook called when sight of the player is lost. Base implementation
 *        intentionally empty as Investigate behavior is already handled by the Behavior
 *        Tree via bIsAlerted and LastKnownPlayerLocation. Override for specific enemy
 *        reactions.
 * 
 * @param[in] LastLocation The last location where the player has been seen
 */
void AEnemyAIController::HandlePlayerLost(FVector LastLocation)
{
    
}

void AEnemyAIController::ExpandSightRadius()
{
    APawn* ControlledPawn = GetPawn();
    AEnemyBase* Enemy = Cast<AEnemyBase>(ControlledPawn);
    if (!IsValid(Enemy) || !IsValid(Enemy->GetDataAsset())) return;

    SightConfig->SightRadius = Enemy->GetDataAsset()->CombatSightRadius;
    SightConfig->LoseSightRadius = SightConfig->SightRadius + Enemy->GetDataAsset()->LoseSightOffset;
    GetPerceptionComponent()->RequestStimuliListenerUpdate();
}

void AEnemyAIController::ResetSightRadius()
{
    APawn* ControlledPawn = GetPawn();
    AEnemyBase* Enemy = Cast<AEnemyBase>(ControlledPawn);
    if (!IsValid(Enemy) || !IsValid(Enemy->GetDataAsset())) return;

    SightConfig->SightRadius = Enemy->GetDataAsset()->DetectionSightRadius;
    SightConfig->LoseSightRadius = SightConfig->SightRadius + Enemy->GetDataAsset()->LoseSightOffset;
    GetPerceptionComponent()->RequestStimuliListenerUpdate();
}