// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingHookComponent.h"
#include "Engine/DamageEvents.h"
#include "CollisionQueryParams.h"
#include "../ProjectAscentCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

#define ECC_GrapplingHook ECC_GameTraceChannel2

// Sets default values for this component's properties
UGrapplingHookComponent::UGrapplingHookComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = EGrapplingState::Idle;
	TargetLocation = FVector::ZeroVector;
	GrapplingHookData = nullptr;
	OwnerCharacter = nullptr;
	CurrentHitResult.Init();

	GrappleBeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GrappleBeamComponent"));

	if (IsValid(GrappleBeamComponent))
	{
		GrappleBeamComponent->SetAutoActivate(false);
		GrappleBeamComponent->SetVisibility(false);
	}

}


// Called when the game starts
void UGrapplingHookComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = EGrapplingState::Idle;
	OwnerCharacter = Cast<AProjectAscentCharacter>(GetOwner());

	if (!IsValid(GrapplingHookData)) return;


	if (IsValid(GrappleBeamComponent))
	{
		GrappleBeamComponent->SetAutoActivate(false);
		GrappleBeamComponent->SetVisibility(false);
	}
}


// Called every frame
void UGrapplingHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState == EGrapplingState::Firing) // Fire Hook
	{
		UpdateHookFiring(DeltaTime);
	}
	else if (CurrentState == EGrapplingState::Flying) // Launching player towards target
	{
		UpdateHookFlying(DeltaTime);
	}
	else if (CurrentState == EGrapplingState::Retracting) // Retracts Hook
	{
		UpdateHookRetracting(DeltaTime);
	}
	else if (CurrentState == EGrapplingState::Pulling)
	{
		UpdateHookPulling(DeltaTime);
	}
}

void UGrapplingHookComponent::UpdateHookFiring(float DeltaTime)
{
	CurrentBeamEnd = FMath::VInterpTo(CurrentBeamEnd, TargetLocation, DeltaTime, GrapplingHookData->FiringInterpSpeed);

	// Set cable start and end in world space
	FVector HandLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("Grapple_Socket"));
	
	GrappleBeamComponent->SetVariableVec3(FName("BeamStart"), HandLocation);
	GrappleBeamComponent->SetVariableVec3(FName("BeamEnd"), CurrentBeamEnd);

	// Check if cable reached target
	float DistanceToTarget = FVector::Dist(CurrentBeamEnd, TargetLocation);

	if (DistanceToTarget < GrapplingHookData->TargetDistanceThreshold)
	{
		// Cable has reached target
		ProcessHookHit();
		
	}
}

void UGrapplingHookComponent::UpdateHookFlying(float DeltaTime)
{
	UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();

	if (!IsValid(MovementComponent)) return;

	FVector Direction = (TargetLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal();
	FVector TargetVelocity = Direction * GrapplingHookData->FlyingSpeed;
	float InterpSpeed = GrapplingHookData->FlyingInterpSpeed;
	
	MovementComponent->Velocity = FMath::VInterpTo(MovementComponent->Velocity, TargetVelocity, DeltaTime, InterpSpeed);

	FVector HandLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("Grapple_Socket"));

	GrappleBeamComponent->SetVariableVec3(FName("BeamStart"), HandLocation);
	GrappleBeamComponent->SetVariableVec3(FName("BeamEnd"), TargetLocation);

	// Check if player has reached target
	float DistanceToTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), TargetLocation);

	if (DistanceToTarget < GrapplingHookData->TargetDistanceThreshold)
	{
		// Player has reached target
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		DisableBeam();
		CurrentState = EGrapplingState::Idle;
	}
}

void UGrapplingHookComponent::UpdateHookRetracting(float DeltaTime)
{
	FVector HandLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("Grapple_Socket"));
	CurrentBeamEnd = FMath::VInterpTo(CurrentBeamEnd, HandLocation, DeltaTime, GrapplingHookData->FiringInterpSpeed);
	
	GrappleBeamComponent->SetVariableVec3(FName("BeamStart"), HandLocation);
	GrappleBeamComponent->SetVariableVec3(FName("BeamEnd"), CurrentBeamEnd);

	// Check if cable reached target
	float DistanceToTarget = FVector::Dist(CurrentBeamEnd, HandLocation);

	if (DistanceToTarget < GrapplingHookData->TargetDistanceThreshold)
	{
		// Beam has reached back to the player
		DisableBeam();
		CurrentState = EGrapplingState::Idle;
	}
}

void UGrapplingHookComponent::UpdateHookPulling(float DeltaTime)
{
	if (!IsValid(PulledActor))
	{
		CurrentState = EGrapplingState::Retracting;
		return;
	}

	// Update pull target to current player location
	FVector PlayerLocation = OwnerCharacter->GetActorLocation();
	IGrappable::Execute_GrapplePull(PulledActor, GrapplingHookData->PropPullSpeed, PlayerLocation);

	// Update cable end to pulled actor location
	FVector HandLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("Grapple_Socket"));
	GrappleBeamComponent->SetVariableVec3(FName("BeamStart"), HandLocation);
	GrappleBeamComponent->SetVariableVec3(FName("BeamEnd"), PulledActor->GetActorLocation());

	// Check if actor reached player
	float DistanceToPlayer = FVector::Dist(PulledActor->GetActorLocation(), PlayerLocation);
	
	if (DistanceToPlayer < GrapplingHookData->TargetDistanceThreshold)
	{
		DisableBeam();
		IGrappable::Execute_GrapplePullCancel(PulledActor);
		PulledActor = nullptr;
		CurrentState = EGrapplingState::Idle;
	}
}

void UGrapplingHookComponent::DisableBeam()
{
	GrappleBeamComponent->Deactivate();
	GrappleBeamComponent->SetVisibility(false);

	CurrentHitResult.Reset();

	OwnerCharacter->bUseControllerRotationYaw = true;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}



void UGrapplingHookComponent::FireHook(FVector TraceStart, FVector TraceEnd)
{
	if (!IsValid(OwnerCharacter) || !CanGrapple() || !IsValid(GrapplingHookData))
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return;
	}

	// Set State
	CurrentState = EGrapplingState::Firing;

	// Shoot Line Trace
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GrapplingHook, CollisionParams);

	GrappleBeamComponent ->SetVisibility(true);

	// Start cable end at hand position
	CurrentBeamEnd = OwnerCharacter->GetMesh()->GetSocketLocation(FName("Grapple_Socket"));

	if (bHit)
	{
		TargetLocation = Hit.ImpactPoint;
		CurrentHitResult = Hit;
	}
	else
	{
		TargetLocation = TraceEnd;
		CurrentHitResult.Reset();
	}

	GrappleBeamComponent->SetVisibility(true);
	GrappleBeamComponent->Activate(true);
}

void UGrapplingHookComponent::RetractHook()
{
	CurrentState = EGrapplingState::Retracting;

	UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();

	if (MovementComponent)
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
	}
}

void UGrapplingHookComponent::ProcessHookHit()
{
	// Hook did not hit anything
	if (!CurrentHitResult.bBlockingHit)
	{
		// Retract hook
		RetractHook();
		return;
	}

	OwnerCharacter->bUseControllerRotationYaw = false;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;

	AActor* HitActor = CurrentHitResult.GetActor();

	if (IsValid(HitActor) && HitActor->GetClass()->ImplementsInterface(UGrappable::StaticClass()))
	{
		// Pull Actor (Prop / Enemy)
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);

		if (HitCharacter) // Pull Actor via impulse
		{
			IGrappable::Execute_GrapplePull(HitActor, GrapplingHookData->EnemyPullForce, OwnerCharacter->GetActorLocation());
		}
		else
		{
			PulledActor = HitActor;
			CurrentState = EGrapplingState::Pulling;

		}
	}
	else
	{
		// Launch player towards target that got hit
		CurrentState = EGrapplingState::Flying;

		UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();

		if (MovementComponent)
		{
			MovementComponent->SetMovementMode(MOVE_Flying);
		}
	}

}

bool UGrapplingHookComponent::CanGrapple()
{
	return CurrentState == EGrapplingState::Idle;
}

float UGrapplingHookComponent::GetGrapplingHookRange()
{
	return GrapplingHookData->MaxRange;
}

EGrapplingState UGrapplingHookComponent::GetGrapplingState()
{
	return CurrentState;
}

