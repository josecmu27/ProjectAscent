// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrapplingHookDataAsset.h"
#include "Grappable.h"
#include "CableComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GrapplingHookComponent.generated.h"

UENUM(BlueprintType)
enum class EGrapplingState : uint8
{
	Idle	   UMETA(DisplayName = "Idle"),
	Firing	   UMETA(DisplayName = "Firing"),
	Flying	   UMETA(DisplayName = "Flying"),
	Retracting UMETA(DisplayName = "Retracting"),
	Pulling	   UMETA(DisplayName = "Pulling")
};

class AProjectAscentCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTASCENT_API UGrapplingHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrapplingHookComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void FireHook(FVector TraceStart, FVector TraceEnd);

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void DisableBeam();

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void RetractHook();

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void ProcessHookHit();

	UFUNCTION(BlueprintPure, Category = "Grappling Hook")
	EGrapplingState GetGrapplingState();


	/* Tick Functions */

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void UpdateHookFiring(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void UpdateHookFlying(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void UpdateHookRetracting(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Grappling Hook")
	void UpdateHookPulling(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Grappling Hook")
	bool CanGrapple();

	UFUNCTION(BlueprintPure, Category = "Grappling Hook")
	float GetGrapplingHookRange();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"));
	UNiagaraComponent* GrappleBeamComponent;

	FVector CurrentBeamEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EGrapplingState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AProjectAscentCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UGrapplingHookDataAsset* GrapplingHookData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FHitResult CurrentHitResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector TargetLocation;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* PulledActor;

};
