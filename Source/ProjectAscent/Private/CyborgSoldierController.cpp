// Fill out your copyright notice in the Description page of Project Settings.


#include "CyborgSoldierController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CyborgSoldier.h"
#include <Kismet/GameplayStatics.h>

void ACyborgSoldierController::HandlePlayerSeen(AActor* Player)
{
    AlertAllSoldiers(Player);
}

void ACyborgSoldierController::HandlePlayerLost(FVector LastLocation)
{
    UBlackboardComponent* BBComponent = GetBlackboardComponent();
    if (!IsValid(BBComponent)) return;

    BBComponent->SetValueAsBool(FName("bIsInvestigating"), true);

    TellNearbySoldiersToInvestigate(LastLocation);
}

/**
 * @brief Alert all soldiers about the spotted player
 *
 * @param[in] Player The player actor
 */
void ACyborgSoldierController::AlertAllSoldiers(AActor* Player)
{
    TArray<AActor*> AllSoldiers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACyborgSoldier::StaticClass(), AllSoldiers);

    for (AActor* Actor : AllSoldiers)
    {
        ACyborgSoldier* Soldier = Cast<ACyborgSoldier>(Actor);
        if (!IsValid(Soldier) || Soldier == GetPawn()) continue;

        ACyborgSoldierController* AIController = Cast<ACyborgSoldierController>(Soldier->GetController());
        if (!IsValid(AIController)) continue;

        UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
        if (!IsValid(BBComponent)) continue;

        BBComponent->SetValueAsObject(FName("TargetActor"), Player);
        BBComponent->SetValueAsBool("bIsAlerted", true);
    }
   
}

/**
 * @brief Set nearby soldiers within a radius to investigate last location where
 *        player has been seen
 *
 * @param[in] LastLocation The last location where the player has been seen
 */
void ACyborgSoldierController::TellNearbySoldiersToInvestigate(FVector LastLocation)
{
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;

    ActorsToIgnore.Add(GetPawn());

    TArray<AActor*> OverlappingActors;

    ACyborgSoldier* OwnerSoldier = Cast<ACyborgSoldier>(GetOwner());

    if (!IsValid(OwnerSoldier) || !IsValid(OwnerSoldier->GetDataAsset())) return;

    // Get all overlapping actors within a radius
    bool bHasOverlaps = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        LastLocation,
        OwnerSoldier->GetDataAsset()->AlertSphereRadius,
        ObjectTypes,
        ACyborgSoldier::StaticClass(),
        ActorsToIgnore,
        OverlappingActors
    );


    if (bHasOverlaps)
    {
        for (AActor* Actor : OverlappingActors)
        {
            ACyborgSoldier* Soldier = Cast<ACyborgSoldier>(Actor);
            if (!IsValid(Soldier)) continue;

            ACyborgSoldierController* AIController = Cast<ACyborgSoldierController>(Soldier->GetController());
            if (!IsValid(AIController)) continue;

            UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
            if (!IsValid(BBComponent)) continue;

            BBComponent->SetValueAsVector(FName("LastKnownPlayerLocation"), LastLocation);
            BBComponent->SetValueAsBool(FName("bIsInvestigating"), true);
        }
    }

}
