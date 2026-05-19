// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(ProjectAscenet), meta=(BlueprintSpawnableComponent) )
class PROJECTASCENT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UFUNCTION(BlueprintPure, Category = "Health")
	float const GetCurHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	float const GetMaxHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	bool const IsDead();


protected:
	virtual void BeginPlay() override;

private:
	float CurHealth;
	float MaxHealth;
	bool bIsDead;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
		
};
