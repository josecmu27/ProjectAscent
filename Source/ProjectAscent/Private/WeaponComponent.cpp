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
* @brief shoots Line Trace(s) from TraceStart to TraceEnd based on weapon's projectiles per shot,
*		 applies Damage to Hit Actor if a hit occurs, manages Weapon's current and reserved ammo,
*		 and broadcast delegates
* @param[in] TraceStart starting vector of Line Trace
* @param[in] TraceEnd ending vector of Line Trace
* 
*/
void UWeaponComponent::Fire(FVector TraceStart, FVector TraceEnd)
{
	if (!CanFire())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World) || !IsValid(WeaponData))
	{
		return;
	}
	
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
* @brief reloads weapon by filling current magazine with reserved bullets
* @param[in] TraceStart starting vector of Line Trace
* @param[in] TraceEnd end vector of Line Trace
*
*/
void UWeaponComponent::Reload()
{
	// Check if we can reload
	if (!CanReload()) return;

	UWorld* World = GetWorld();

	if (!IsValid(World) || !IsValid(WeaponData)) return;

	// Set Current State to Reloading
	CurrentState = EWeaponState::Reloading;

	// Decrease Ammo Reserve Count
	int32 ReloadedAmmo = (ReserveAmmo >= WeaponData->MagazineSize - CurrentAmmo) ? WeaponData->MagazineSize - CurrentAmmo : ReserveAmmo;

	CurrentAmmo += ReloadedAmmo;
	ReserveAmmo -= ReloadedAmmo;

	OnReloadStarted.Broadcast(WeaponData->WeaponType);
	OnAmmoUpdated.Broadcast();

	World->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UWeaponComponent::OnReloadTimerExpired, WeaponData->ReloadTime, false);
}

void UWeaponComponent::OnFireRateTimerExpired()
{
	CurrentState = EWeaponState::Idle;
}

void UWeaponComponent::OnReloadTimerExpired()
{
	CurrentState = EWeaponState::Idle;
}

bool UWeaponComponent::CanFire() const
{
	return (CurrentState == EWeaponState::Idle && CurrentAmmo > 0);
}

bool UWeaponComponent::CanReload() const
{
	return (ReserveAmmo > 0) && (CurrentAmmo< WeaponData->MagazineSize) && (CurrentState == EWeaponState::Idle);
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