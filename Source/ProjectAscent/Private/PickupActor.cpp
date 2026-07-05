// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"
#include "ProjectAscent/ProjectAscentCharacter.h"

// Sets default values
APickupActor::APickupActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->InitSphereRadius(75.0f);

}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(SphereComponent)) SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::HandleSphereOverlap);
	
}

void APickupActor::HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APickupActor::OnPickedUp_Implementation()
{
	Destroy();
}
