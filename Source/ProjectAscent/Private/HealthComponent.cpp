// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	CurHealth = 100.0f;
	MaxHealth = 100.0f;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

float const UHealthComponent::GetCurHealth()
{
	return CurHealth;
}

float const UHealthComponent::GetMaxHealth()
{
	return MaxHealth;
}

