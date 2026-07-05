// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "TimerManager.h"
#include "Engine/DamageEvents.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

#define ECC_Weapon ECC_GameTraceChannel1

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	CurrentState = EWeaponState::Idle;
	CurrentAmmo = 25;
	ReserveAmmo = 25;
	FireRateTimerHandle;
	ReloadTimerHandle;
	WeaponData = nullptr;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(WeaponData)) return;

	CurrentAmmo = WeaponData->MagazineSize;
	CurrentState = EWeaponState::Idle;
}


/*////////////////////////////////////////////////////////////////////
------------------------ Private Functions ---------------------------
*/////////////////////////////////////////////////////////////////////


/*
* @brief Shoots Line Trace(s) from TraceStart to TraceEnd based on weapon's projectiles per shot,
*		 and applies Damage to Hit Actor if a hit occurs. Manages Weapon's current and reserved ammo,
*		 and broadcast OnFireStarted and OnAmmoUpdated on success
* @param[in] TraceStart starting vector of Line Trace
* @param[in] TraceEnd ending vector of Line Trace
* 
*/
void UWeaponComponent::Fire(FVector TraceStart, FVector TraceEnd)
{
	// Interrupt reload if mid-reload, allow firing to cancel reloading
	if (CurrentState == EWeaponState::Reloading)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		CurrentState = EWeaponState::Idle;
	}

	if (!CanFire()) return;

	UWorld* World = GetWorld();

	if (!IsValid(World) || !IsValid(WeaponData)) return;
	
	// Set Current State to Firing
	CurrentState = EWeaponState::Firing;
	FVector BaseDirection = (TraceEnd - TraceStart).GetSafeNormal();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());


	for (int32 i = 0; i < WeaponData->ProjectilesPerShot; i++)
	{
		FVector ProjectileDirection = FMath::VRandCone(BaseDirection, FMath::DegreesToRadians(WeaponData->SpreadAngle) * 0.5f);
		FVector ProjectileTraceEnd = TraceStart + (ProjectileDirection * WeaponData->Range);

		FHitResult Hit;

		// Shoot Line Trace
		bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, ProjectileTraceEnd, ECC_Weapon, CollisionParams);

		// Check if Line Trace hits and apply damage to it via unreal's damage system
		if (bHit && Hit.GetActor())
		{
			AActor* HitActor = Hit.GetActor();

			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.HitInfo = Hit;
			PointDamageEvent.ShotDirection = ProjectileDirection;

			AController* InstigatorController = GetOwner()->GetInstigatorController();

			HitActor->TakeDamage(WeaponData->Damage, PointDamageEvent, InstigatorController, GetOwner());

			OnWeaponHit.Broadcast(Hit);

		}
	}

	// Decrease Current Projectile Count
	CurrentAmmo = FMath::Max(CurrentAmmo - 1, 0);

	CurrentState = (CurrentAmmo == 0) ? EWeaponState::Empty : EWeaponState::Firing;

	// Broadcast Delegates
	OnFireStarted.Broadcast(WeaponData->WeaponType); // Start player firing animation
	OnAmmoUpdated.Broadcast(); // Update Weapon Ammo UI


	// Set FireRateTimer
	World->GetTimerManager().SetTimer(FireRateTimerHandle, this, &UWeaponComponent::OnFireRateTimerExpired, WeaponData->FireRate, false);
}

/*
* @brief Reloads weapon based on reserve ammo and weapon magazine size. 
*        Broadcasts OnReloadStarted and OnAmmoUpdated on success.
* @param[in] TraceStart starting vector of Line Trace
* @param[in] TraceEnd end vector of Line Trace
*
*/
void UWeaponComponent::Reload()
{
	if (!CanReload()) return;

	CurrentState = EWeaponState::Reloading;
	OnReloadStarted.Broadcast(WeaponData->WeaponType);

	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UWeaponComponent::OnReloadCycle,
		WeaponData->ReloadCycleRate,
		true  // loop stops when full or out of reserve
	);
}

void UWeaponComponent::OnFireRateTimerExpired()
{
	CurrentState = EWeaponState::Idle;
}

void UWeaponComponent::OnReloadCycle()
{
	int32 AmmoNeeded = WeaponData->MagazineSize - CurrentAmmo;
	int32 AmmoReloaded = FMath::Min3(AmmoNeeded, ReserveAmmo, WeaponData->AmmoReloadedPerCycle);
	
	CurrentAmmo += AmmoReloaded;
	ReserveAmmo -= AmmoReloaded;

	OnReloadCycleStarted.Broadcast(WeaponData->WeaponType);
	OnAmmoUpdated.Broadcast();

	// Stop looping when full or out of reserve ammo
	if (CurrentAmmo >= WeaponData->MagazineSize || ReserveAmmo <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		CurrentState = EWeaponState::Idle;
		OnReloadEnded.Broadcast(WeaponData->WeaponType);
	}
}

bool UWeaponComponent::CanFire() const
{
	return (CurrentState == EWeaponState::Idle && CurrentAmmo > 0);
}

bool UWeaponComponent::CanReload() const
{
	return (ReserveAmmo > 0) && (CurrentAmmo < WeaponData->MagazineSize) && (CurrentState == EWeaponState::Idle);
}




/*////////////////////////////////////////////////////////////////////
------------------------ Public Functions ---------------------------
*/////////////////////////////////////////////////////////////////////

int32 UWeaponComponent::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

float UWeaponComponent::GetWeaponRange() const
{
	if (!IsValid(WeaponData)) return 0.0f;

	return WeaponData->Range;
}

EWeaponState UWeaponComponent::GetWeaponState() const
{
	return CurrentState;
}

UWeaponDataAsset* UWeaponComponent::GetWeaponData() const
{
	return WeaponData;
}

void UWeaponComponent::SetWeaponData(UWeaponDataAsset* NewWeaponData)
{
	WeaponData = NewWeaponData;
}

bool UWeaponComponent::AddReserveAmmo(int32 Amount)
{
	if (!IsValid(WeaponData)) return false;
	if (ReserveAmmo >= WeaponData->MaxReserveAmmo) return false; // already full

	ReserveAmmo = FMath::Min(ReserveAmmo + Amount, WeaponData->MaxReserveAmmo);

	OnAmmoUpdated.Broadcast();

	return true;
}