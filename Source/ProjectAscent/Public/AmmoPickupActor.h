// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupActor.h"
#include "WeaponTypes.h"
#include "AmmoPickupActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API AAmmoPickupActor : public APickupActor
{
	GENERATED_BODY()
	
public: 

	UPROPERTY(EditAnywhere, Category = "Ammo")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 AmmoAmount;

protected:
	virtual void HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
