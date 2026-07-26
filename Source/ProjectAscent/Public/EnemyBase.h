// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthComponent.h"
#include "Grappable.h"
#include "EnemyDataAsset.h"
#include "CombatRoleSubsystem.h"
#include "EnemyBase.generated.h"

UCLASS()
class PROJECTASCENT_API AEnemyBase : public ACharacter, public IGrappable
{
	GENERATED_BODY()

public:
	AEnemyBase();

	/* ---------- Grappable Interface ---------- */
	virtual void GrapplePull_Implementation(float Magnitude, FVector PlayerLocation) override;
	virtual void GrapplePullCancel_Implementation() override;


	/* ---------- Attack Virtual Functions ---------- */
	virtual void Attack();
	virtual bool CanAttack() const;
	virtual float GetAttackRange() const;


	/* ---------- Token Subsystem Functions ---------- */
	virtual float GetTokenPriorityScore(float WaitTime) const;
	virtual void AcceptToken();
	virtual void ClearToken();
	virtual void SurrenderToken();


	/* ---------- Combat Role ---------- */
	ECombatRole GetCombatRole() const { return CombatRole; }
	void SetCombatRole(ECombatRole NewCombatRole) { CombatRole = NewCombatRole; }


	/* ---------- Aiming ---------- */
	UFUNCTION(BlueprintCallable)
	void SetIsAiming(bool bNewIsAiming) { bIsAiming = bNewIsAiming; }

	UFUNCTION(BlueprintCallable)
	bool GetIsAiming() const { return bIsAiming; }


	/* ---------- Data / Death ---------- */
	UFUNCTION(BlueprintCallable)
	UEnemyDataAsset* GetDataAsset() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
	void OnEnemyDeath();
	virtual void OnEnemyDeath_Implementation();


	/* ---------- Aim -------------*/
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FRotator AimRotation;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UEnemyDataAsset* EnemyData;

	/* ---------- Token priority scoring ---------- */
	virtual float GetReadinessScore() const;

private:
	UFUNCTION()
	void HandleDeath();

	UPROPERTY(VisibleAnywhere)
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	ECombatRole CombatRole;

	float GetEngagerReadinessScore() const;
	float GetDefenderReadinessScore() const;
};