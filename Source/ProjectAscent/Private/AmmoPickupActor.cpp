// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickupActor.h"
#include "ProjectAscent/ProjectAscentCharacter.h"


void AAmmoPickupActor::HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    AProjectAscentCharacter* Player = Cast<AProjectAscentCharacter>(OtherActor);
    if (!IsValid(Player) || !IsValid(Player->GetInventoryComponent())) return;

    UWeaponComponent* Weapon = Player->GetInventoryComponent()->GetWeaponByType(WeaponType);
    if (!IsValid(Weapon)) return;

    bool bAccepted = Weapon->AddReserveAmmo(AmmoAmount);
    
    if (bAccepted)
    {
        OnPickedUp();
    }
}
