// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDataAsset.h"
#include "Equippable.h"
#include "WeaponTypes.h"
#include "WeaponComponent.generated.h"


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Firing		UMETA(DisplayName = "Firing"),
	Reloading	UMETA(DisplayName = "Reloading"),
	Empty		UMETA(DisplayName = "Empty")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireStarted, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStarted, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadCycleStarted, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadEnded, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHit, FHitResult, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTASCENT_API UWeaponComponent : public UActorComponent, public IEquippable
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetWeaponRange() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponState GetWeaponState() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UWeaponDataAsset* GetWeaponData() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponData(UWeaponDataAsset* NewWeaponData);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool AddReserveAmmo(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire(FVector TraceStart, FVector TraceEnd);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadStarted OnReloadStarted;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadStarted OnReloadCycleStarted;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadEnded OnReloadEnded;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnFireStarted OnFireStarted;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnWeaponHit OnWeaponHit;

	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnAmmoUpdated OnAmmoUpdated;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EWeaponState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ReserveAmmo;

	FTimerHandle FireRateTimerHandle;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponData;

	UFUNCTION()
	void OnFireRateTimerExpired();

	UFUNCTION()
	void OnReloadCycle();

	bool CanFire() const;

	bool CanReload() const;

};
