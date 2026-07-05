// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    MaxWeaponCount = 2;

    WeaponSlots.Init(nullptr, MaxWeaponCount);
    ActiveWeapon = nullptr;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

/**
 * @brief Creates a new UWeaponComponent from the Data Asset input, registers it on the owning 
 *        actor, and places it into the specified inventory slot. Broadcasts OnActiveWeaponChanged 
 *        and OnNewWeaponEquipped on success.
 *
 * @param[in] NewWeaponData The Data Asset defining the weapon's stats, mesh, and behavior
 * @param[in] Slot The inventory slot to place this weapon into (SlotOne or SlotTwo)
 * @return Pointer to the newly created UWeaponComponent, nullptr if NewWeaponData is invalid
 * 
 */
UWeaponComponent* UInventoryComponent::EquipNewWeapon(UWeaponDataAsset* NewWeaponData, EWeaponSlot Slot)
{
    if (!IsValid(NewWeaponData)) return nullptr;

    UWeaponComponent* NewWeapon = NewObject<UWeaponComponent>(GetOwner());
    NewWeapon->SetWeaponData(NewWeaponData);
    NewWeapon->RegisterComponent();
    NewWeapon->InitializeComponent();

    SetWeaponAtSlot(NewWeapon, Slot);
    ActiveWeapon = NewWeapon;

    OnActiveWeaponChanged.Broadcast();
    OnNewWeaponEquipped.Broadcast(NewWeapon, Slot);

    return NewWeapon;
}


UWeaponComponent* UInventoryComponent::GetActiveWeapon() const
{
    return ActiveWeapon;
}

void UInventoryComponent::SwitchWeapon()
{
    UWeaponComponent* WeaponOne = GetWeaponAtSlot(EWeaponSlot::SlotOne);
    UWeaponComponent* WeaponTwo = GetWeaponAtSlot(EWeaponSlot::SlotTwo);

    if (!IsValid(WeaponOne) || !IsValid(WeaponOne->GetWeaponData())) return;
    if (!IsValid(WeaponTwo) || !IsValid(WeaponTwo->GetWeaponData())) return;
    
    ActiveWeapon = (ActiveWeapon == WeaponOne) ? WeaponTwo : WeaponOne;

    OnActiveWeaponChanged.Broadcast();
}

UWeaponComponent* UInventoryComponent::GetWeaponAtSlot(EWeaponSlot Slot) const
{
    int32 Index = static_cast<int32>(Slot);
    if (!WeaponSlots.IsValidIndex(Index)) return nullptr;

    return WeaponSlots[Index];
}

/**
 * @brief Places weapon component into the specified slot after unregistering any
 *        existing weapon in that slot first. Binds the new weapon's fire, reload,
 *        and ammo delegates to Inventory's delegate handlers so the owning character
 *        only needs to subscribe to Inventory once regardless of how many new weapons are equipped
 *
 * @param[in] WeaponComponent The already constructed weapon component to place in the slot
 * @param[in] Slot            The target inventory slot
 */
void UInventoryComponent::SetWeaponAtSlot(UWeaponComponent* WeaponComponent, EWeaponSlot Slot)
{
    int32 Index = static_cast<int32>(Slot);
    if (!WeaponSlots.IsValidIndex(Index)) return;

    if (IsValid(WeaponSlots[Index]))
    {
        WeaponSlots[Index]->UnregisterComponent();
    }
    WeaponSlots[Index] = WeaponComponent;

    if (IsValid(WeaponComponent))
    {
        WeaponComponent->OnFireStarted.AddDynamic(this, &UInventoryComponent::HandleActiveWeaponFire);
        WeaponComponent->OnReloadStarted.AddDynamic(this, &UInventoryComponent::HandleActiveWeaponReloadStarted);
        WeaponComponent->OnReloadCycleStarted.AddDynamic(this, &UInventoryComponent::HandleActiveWeaponReloadCycle);
        WeaponComponent->OnReloadEnded.AddDynamic(this, &UInventoryComponent::HandleActiveWeaponReloadEnded);
        WeaponComponent->OnAmmoUpdated.AddDynamic(this, &UInventoryComponent::HandleInventoryUpdate);
    }
}

/**
 * @brief Clears the specified slot by unregistering its weapon component and setting the 
 *        slot to nullptr. Intended for weapon drop/swap scenarios where a slot needs to
 *        be emptied before a new weapon is equipped.
 *        Future: spawn AWeaponPickupActor at player location before clearing,
 *        representing the dropped weapon as a world pickup.
 *
 * @param[in] Slot The inventory slot to clear
 */
void UInventoryComponent::ClearWeaponAtSlot(EWeaponSlot Slot)
{
    if (WeaponSlots.Num() == 0) return;

    int32 Index = static_cast<int32>(Slot);
    if (!WeaponSlots.IsValidIndex(Index)) return;

    if (IsValid(WeaponSlots[Index]))
    {
        WeaponSlots[Index]->UnregisterComponent();
    }

    WeaponSlots[Index] = nullptr;
}

/**
 * @brief Searches all weapon slots for a weapon matching the specified type.
 *        Used by AAmmoPickupActor to find the correct weapon to refill without
 *        needing to know which slot it occupies.
 *        O(n) where n = MaxWeaponCount (currently 2), may change after playtesting
 *
 * @param[in] WeaponType The EWeaponType to search for (Rifle, Shotgun, etc.)
 * @return Pointer to the matching UWeaponComponent, or nullptr if not found
 */
UWeaponComponent* UInventoryComponent::GetWeaponByType(EWeaponType WeaponType) const
{
    for (UWeaponComponent* Weapon : WeaponSlots)
    {
        if (IsValid(Weapon) && IsValid(Weapon->GetWeaponData())
            && Weapon->GetWeaponData()->WeaponType == WeaponType)
        {
            return Weapon;
        }
    }
    return nullptr;
}

/////////////////////////////////////////////////////////////////////////
//                      Delegate Handlers                             // 
///////////////////////////////////////////////////////////////////////

void UInventoryComponent::HandleActiveWeaponFire(EWeaponType WeaponType)
{
    OnActiveWeaponFire.Broadcast(WeaponType);
}

void UInventoryComponent::HandleActiveWeaponReloadStarted(EWeaponType WeaponType)
{
    OnActiveWeaponReloadStarted.Broadcast(WeaponType);
}

void UInventoryComponent::HandleActiveWeaponReloadCycle(EWeaponType WeaponType)
{
    OnActiveWeaponReloadCycle.Broadcast(WeaponType);
}

void UInventoryComponent::HandleActiveWeaponReloadEnded(EWeaponType WeaponType)
{
    OnActiveWeaponReloadEnded.Broadcast(WeaponType);
}


void UInventoryComponent::HandleInventoryUpdate()
{
    OnInventoryUpdated.Broadcast();
}

