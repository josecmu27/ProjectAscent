// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

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

void AEnemyBase::HandleDeath()
{
    OnEnemyDeath();
}

void AEnemyBase::OnEnemyDeath_Implementation()
{
    Destroy();
}

UEnemyDataAsset* AEnemyBase::GetDataAsset() const
{
	return EnemyData;
}

void AEnemyBase::GrapplePull_Implementation(float Magnitude, FVector PlayerLocation)
{
    FVector Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
    LaunchCharacter(Direction * Magnitude, true, true);
}

void AEnemyBase::GrapplePullCancel_Implementation()
{
  
}

void AEnemyBase::Attack()
{

}
