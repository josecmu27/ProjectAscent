// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	CurHealth = 100.0f;
	MaxHealth = 100.0f;
	bIsDead = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}


	Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (IsDead() || Damage <= 0.0f)
	{
		return;
	}

	CurHealth = FMath::Max(CurHealth - Damage, 0.0f);

	if (CurHealth <= 0.0f)
	{
		bIsDead = true;
	}
}

float const UHealthComponent::GetCurHealth()
{
	return CurHealth;
}

float const UHealthComponent::GetMaxHealth()
{
	return MaxHealth;
}

bool const UHealthComponent::IsDead()
{
	return bIsDead;
}

