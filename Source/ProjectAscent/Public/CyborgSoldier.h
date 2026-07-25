// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "WeaponComponent.h"
#include "CyborgSoldier.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API ACyborgSoldier : public AEnemyBase
{
	GENERATED_BODY()

public:

	ACyborgSoldier();

	
	void Attack() override;

	bool CanAttack() const override;

	float GetAttackRange() const override;

	UFUNCTION()
	void FireWeapon();
	
	UFUNCTION()
	void ReloadWeapon();

	bool IsReloading() const;

private:
	/* Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWeaponComponent* WeaponComponent;

};
