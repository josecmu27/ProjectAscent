// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"

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

	CurHealth = MaxHealth;

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

	// Report hit to perception so the AI can react even without line of sight
	UAIPerceptionSystem::GetCurrent(GetWorld())->OnEvent(
		FAIDamageEvent(DamagedActor, DamageCauser, Damage, DamagedActor->GetActorLocation()));

	if (CurHealth <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}

}

float UHealthComponent::GetCurHealth() const
{
	return CurHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

bool UHealthComponent::IsDead() const
{
	return bIsDead;
}

