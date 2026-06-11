// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"
#include "GameFramework/Actor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsPulling = false;
	PullSpeed = 0.0f;
	PullTargetLocation = FVector::ZeroVector;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

void AInteractableActor::Tick(float DeltaTime)
{
	if (bIsPulling)
	{
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), PullTargetLocation, DeltaTime, PullSpeed);

		SetActorLocation(NewLocation);

		if (FVector::Dist(GetActorLocation(), PullTargetLocation) < TargetDistanceThreshold)
		{
			bIsPulling = false;
		}
	}

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeath.AddDynamic(this, &AInteractableActor::HandleDeath);
	}
	
}

void AInteractableActor::GrapplePull_Implementation(float Magnitude, FVector PlayerLocation)
{
	bIsPulling = true;
	PullTargetLocation = PlayerLocation;
	PullSpeed = Magnitude;
	MeshComponent->SetSimulatePhysics(false);
}

void AInteractableActor::GrapplePullCancel_Implementation()
{
	bIsPulling = false;
	MeshComponent->SetSimulatePhysics(true);
}

void AInteractableActor::OnInteractableDestroyed_Implementation()
{
	Destroy();
}

void AInteractableActor::HandleDeath()
{
	OnInteractableDestroyed();
}