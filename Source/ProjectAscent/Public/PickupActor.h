// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include "PickupActor.generated.h"

UCLASS()
class PROJECTASCENT_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupActor();

	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickedUp();

	virtual void OnPickedUp_Implementation();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;

private:
};
