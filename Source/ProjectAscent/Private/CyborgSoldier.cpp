// Fill out your copyright notice in the Description page of Project Settings.


#include "CyborgSoldier.h"
#include "CyborgSoldierController.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"



ACyborgSoldier::ACyborgSoldier()
{
    WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
}

void ACyborgSoldier::Attack()
{
    FireWeapon();
}

void ACyborgSoldier::FireWeapon()
{

    if (WeaponComponent->GetCurBullets() == 0)
    {
        ReloadWeapon();
        return;
    }

    FVector TraceStart = GetMesh()->GetSocketLocation(FName("WeaponSocket"));
    FVector TraceDirection = AimRotation.Vector();
    FVector TraceEnd = TraceStart + (TraceDirection * WeaponComponent->GetWeaponRange());

    WeaponComponent->Fire(TraceStart, TraceEnd);
}

void ACyborgSoldier::ReloadWeapon()
{
    WeaponComponent->Reload();
}
