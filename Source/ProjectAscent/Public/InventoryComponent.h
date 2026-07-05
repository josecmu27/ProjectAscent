// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiveWeaponChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponFire, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponReloadStarted, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponReloadEnded, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponReloadCycle, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewWeaponEquipped, UWeaponComponent*, NewWeapon, EWeaponSlot, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTASCENT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwitchWeapon();

	/* Equip function(s) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UWeaponComponent* EquipNewWeapon(UWeaponDataAsset* NewWeaponData, EWeaponSlot Slot);

	/* Weapon Management */

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UWeaponComponent* GetActiveWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UWeaponComponent* GetWeaponAtSlot(EWeaponSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetWeaponAtSlot(UWeaponComponent* WeaponComponent, EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearWeaponAtSlot(EWeaponSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UWeaponComponent* GetWeaponByType(EWeaponType WeaponType) const;


	/* Delegates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnActiveWeaponChanged OnActiveWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnActiveWeaponFire OnActiveWeaponFire;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnActiveWeaponReloadStarted OnActiveWeaponReloadStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActiveWeaponReloadCycle OnActiveWeaponReloadCycle;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnActiveWeaponReloadStarted OnActiveWeaponReloadEnded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnNewWeaponEquipped OnNewWeaponEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Iventory")
	FOnInventoryUpdated OnInventoryUpdated;


protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<UWeaponComponent*> WeaponSlots;

private:	
	UPROPERTY()
	UWeaponComponent* ActiveWeapon;

	int32 MaxWeaponCount;


	/* Delegate Handlers */
	UFUNCTION()
	void HandleActiveWeaponFire(EWeaponType WeaponType);

	UFUNCTION()
	void HandleActiveWeaponReloadStarted(EWeaponType WeaponType);

	UFUNCTION()
	void HandleActiveWeaponReloadCycle(EWeaponType WeaponType);

	UFUNCTION()
	void HandleActiveWeaponReloadEnded(EWeaponType WeaponType);

	UFUNCTION()
	void HandleInventoryUpdate();


};
