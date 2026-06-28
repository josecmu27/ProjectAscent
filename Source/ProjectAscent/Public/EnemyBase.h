// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthComponent.h"
#include "Grappable.h"
#include "EnemyDataAsset.h"
#include "EnemyBase.generated.h"

UCLASS()
class PROJECTASCENT_API AEnemyBase : public ACharacter, public IGrappable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	/* Grappable Interface */
	virtual void GrapplePull_Implementation(float Magnitude, FVector PlayerLocation) override;

	virtual void GrapplePullCancel_Implementation() override;

	UFUNCTION()
	virtual void Attack();

	UEnemyDataAsset* GetDataAsset() const;

	UFUNCTION(BlueprintCallable)
	void SetIsAiming(bool bNewIsAiming) { bIsAiming = bNewIsAiming; }

	UFUNCTION(BlueprintCallable)
	bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
	void OnEnemyDeath();

	virtual void OnEnemyDeath_Implementation();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FRotator AimRotation;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	/* Data Asset*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UEnemyDataAsset* EnemyData;

private:
	
	/* Delegate Handlers */
	UFUNCTION()
	void HandleDeath();

	UPROPERTY(VisibleAnywhere)
	bool bIsAiming;

};
