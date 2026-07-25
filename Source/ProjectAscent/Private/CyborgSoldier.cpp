// Fill out your copyright notice in the Description page of Project Settings.


#include "CyborgSoldier.h"
#include "CyborgSoldierController.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include <CombatTokenSubsystem.h>



ACyborgSoldier::ACyborgSoldier()
{
    WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
}

void ACyborgSoldier::Attack()
{
    FireWeapon();
}

bool ACyborgSoldier::CanAttack() const
{
    return !IsReloading();
}

float ACyborgSoldier::GetAttackRange() const
{
    if (!IsValid(WeaponComponent) || !IsValid(WeaponComponent->GetWeaponData())) return 0.0f;

    return WeaponComponent->GetWeaponData()->Range;
}


void ACyborgSoldier::FireWeapon()
{

    if (WeaponComponent->GetCurrentAmmo() == 0)
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


bool ACyborgSoldier::IsReloading() const
{
    return WeaponComponent && WeaponComponent->GetWeaponState() == EWeaponState::Reloading;
}