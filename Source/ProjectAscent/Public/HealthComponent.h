// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTASCENT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(sBlueprintPure, Category = "Health")
	bool IsDead() const;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings", meta = (AllowPrivateAccess = "true"))
	float CurHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
		
};
