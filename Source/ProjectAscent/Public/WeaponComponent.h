// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDataAsset.h"
#include "Equippable.h"
#include "WeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Firing		UMETA(DisplayName = "Firing"),
	Reloading	UMETA(DisplayName = "Reloading"),
	Empty		UMETA(DisplayName = "Empty")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHit, FHitResult, Hit);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTASCENT_API UWeaponComponent : public UActorComponent, public IEquippable
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurBullets() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetReserveBullets() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetWeaponRange() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponState GetWeaponState() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire(FVector TraceStart, FVector TraceEnd);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadStarted OnReloadStarted;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnFireStarted OnFireStarted;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnWeaponHit OnWeaponHit;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCanFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EWeaponState CurState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CurBullets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ReserveBullets;

	FTimerHandle FireRateTimerHandle;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponData;

	UFUNCTION()
	void OnFireRateTimerExpired();

	UFUNCTION()
	void OnReloadTimerExpired();

	bool CanFire() const;

	bool CanReload() const;

};
