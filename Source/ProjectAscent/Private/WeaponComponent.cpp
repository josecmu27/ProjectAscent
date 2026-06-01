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
	bCanFire = true;
	CurState = EWeaponState::Idle;
	CurBullets = 25;
	ReserveBullets = 25;
	FireRateTimerHandle;
	ReloadTimerHandle;
	WeaponData = nullptr;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(WeaponData))
	{
		return;
	}

	CurBullets = WeaponData->MagazineSize;
	CurState = EWeaponState::Idle;
	bCanFire = true;
}


/*////////////////////////////////////////////////////////////////////
------------------------ Private Functions ---------------------------
*/////////////////////////////////////////////////////////////////////


/*
* @brief shoots Line Trace from TraceStart to TraceEnd,
*		 applies Damage to Hit Actor if a hit occurs,
*		 manages Weapon's current and reserved bullets
* @param[in] TraceStart starting vector of Line Trace
* @param[in] TraceEnd ending vector of Line Trace
* 
*/
void UWeaponComponent::Fire(FVector TraceStart, FVector TraceEnd)
{
	// Check if we can fire
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
	CurState = EWeaponState::Firing;
	bCanFire = false;
	
	// Shoot Line Trace
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Weapon, CollisionParams);

	// Broadcast Delegate
	OnFireStarted.Broadcast();


	// Check if Line Trace hits and apply damage to it via unreal's damage system
	if (bHit && Hit.GetActor())
	{

		AActor* HitActor = Hit.GetActor();

		FPointDamageEvent PointDamageEvent;
		PointDamageEvent.HitInfo = Hit;
		PointDamageEvent.ShotDirection = (TraceEnd - TraceStart).GetSafeNormal();

		AController* InstigatorController = GetOwner()->GetInstigatorController();

		HitActor->TakeDamage(WeaponData->Damage, PointDamageEvent, InstigatorController, GetOwner());
		
		OnWeaponHit.Broadcast(Hit);
	}

	// Decrease Current Bullet Count
	CurBullets = FMath::Max(CurBullets - 1, 0);
	

	CurState = (CurBullets == 0) ? EWeaponState::Empty : EWeaponState::Idle;

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
	if (!CanReload())
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World) || !IsValid(WeaponData))
	{
		return;
	}

	// Set Current State to Reloading
	CurState = EWeaponState::Reloading;
	bCanFire = false;

	// Decrease Bullet Reserve Count
	int32 ReloadedBullets = (ReserveBullets >= WeaponData->MagazineSize) ? WeaponData->MagazineSize : ReserveBullets;

	CurBullets = ReloadedBullets;
	ReserveBullets -= ReloadedBullets;

	OnReloadStarted.Broadcast();


	World->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UWeaponComponent::OnReloadTimerExpired, WeaponData->ReloadTime, false);
}

void UWeaponComponent::OnFireRateTimerExpired()
{
	bCanFire = true;
	CurState = EWeaponState::Idle;
}

void UWeaponComponent::OnReloadTimerExpired()
{
	bCanFire = true;
	CurState = EWeaponState::Idle;
}

bool UWeaponComponent::CanFire() const
{
	return (bCanFire && CurState == EWeaponState::Idle && CurBullets > 0);
}

bool UWeaponComponent::CanReload() const
{
	return ReserveBullets > 0 && CurBullets < WeaponData->MagazineSize && CurState == EWeaponState::Idle;
}




/*////////////////////////////////////////////////////////////////////
------------------------ Public Functions ---------------------------
*/////////////////////////////////////////////////////////////////////

int32 UWeaponComponent::GetCurBullets() const
{
	return CurBullets;
}

int32 UWeaponComponent::GetReserveBullets() const
{
	return ReserveBullets;
}

float UWeaponComponent::GetWeaponRange() const
{
	if (!IsValid(WeaponData))
	{
		return 0.0f;
	}

	return WeaponData->Range;
}

bool UWeaponComponent::IsFiring() const
{
	return CurState == EWeaponState::Firing;
}

bool UWeaponComponent::IsReloading() const
{
	return CurState == EWeaponState::Reloading;
}