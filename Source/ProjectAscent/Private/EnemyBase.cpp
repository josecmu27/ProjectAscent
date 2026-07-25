// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "EnemyAIController.h"
#include "CombatTokenSubsystem.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	AimRotation = FRotator::ZeroRotator;
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyBase::HandleDeath);
	}

}

/* ------------------- DATA FUNCTIONS --------------------*/
UEnemyDataAsset* AEnemyBase::GetDataAsset() const
{
	return EnemyData;
}

/* -------------------------- GRAPPABLE INTERFACE -------------------------- */
void AEnemyBase::GrapplePull_Implementation(float Magnitude, FVector PlayerLocation)
{
	FVector Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
	LaunchCharacter(Direction * Magnitude, true, true);
}

void AEnemyBase::GrapplePullCancel_Implementation()
{

}

/* ------------------- ATTACK VIRTUAL FUNCTIONS ------------ */
void AEnemyBase::Attack()
{

}

bool AEnemyBase::CanAttack() const
{
	return true;
}

float AEnemyBase::GetAttackRange() const
{
	return 0.0f;
}

/* ---------- TOKEN SUBSYSTEM FUNCTIONS ---------- */

void AEnemyBase::AcceptToken()
{
	if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
	{
		AIController->SetCombatToken(true);
	}
}

void AEnemyBase::ClearToken()
{
	if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
	{
		AIController->SetCombatToken(false);
	}
}

void AEnemyBase::SurrenderToken()
{
	if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
	{
		AIController->SetCombatToken(false);
	}
}

/**
 * @brief Calculates token priority score based on WaitTime and Position
 *
 * @param[in] WaitTime The time the enemy has been waiting as a requester
 */
float AEnemyBase::GetTokenPriorityScore(float WaitTime) const
{
	if (!EnemyData) return 0.0f;

	return (EnemyData->WaitWeight * WaitTime) + (EnemyData->PositionWeight * GetReadinessScore());
}

/**
 * @brief Scores how ready an enemy is to attack based on their combat role. 
 */
float AEnemyBase::GetReadinessScore() const
{
	switch (CombatRole)
	{
		case ECombatRole::Engager:  return GetEngagerReadinessScore();
		case ECombatRole::Defender: return GetDefenderReadinessScore();
		default:                   return 0.0f;
	}
}

/**
 * @brief Scores how ready an Engager is to attack based on proximity to its target.
 *        Returns a value in [0, 1], where 1.0 means at or inside effective attack
 *        range and 0.0 means at or beyond it.
 */
float AEnemyBase::GetEngagerReadinessScore() const
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (!AIController) return 0.0f;

	AActor* Target = AIController->GetTargetActor();
	if (!Target) return 0.0f;

	float AttackRange = GetAttackRange();
	if (AttackRange <= 0.0f) return 0.0f;

	float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	return FMath::Clamp(1.0f - (Distance / AttackRange), 0.0f, 1.0f);
}

/**
 * @brief Scores how ready a Defender is to attack based on line of sight to its target.
 *        Returns 1.0 if it currently has line of sight, 0.0 otherwise.
 */
float AEnemyBase::GetDefenderReadinessScore() const
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (!AIController) return 0.0f;

	return AIController->GetCanSeePlayer() ? 1.0f : 0.0f;
}

void AEnemyBase::HandleDeath()
{
	if (UCombatTokenSubsystem* TokenSubsystem = GetWorld()->GetSubsystem<UCombatTokenSubsystem>())
	{
		TokenSubsystem->ReturnToken(this);

	}

	OnEnemyDeath(); // Blueprint Visuals
}

void AEnemyBase::OnEnemyDeath_Implementation()
{
	Destroy();
}
