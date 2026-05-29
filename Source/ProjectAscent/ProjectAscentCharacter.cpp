// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectAscentCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AProjectAscentCharacter

AProjectAscentCharacter::AProjectAscentCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	WeaponSocketName = "ik_hand_r";
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName);

	// ADS Default Values
	HipFireArmLength = 400.0f;
	AimArmLength = 200.0f;
	HipFireFOV = 90.0f;
	AimFOV = 60.0f;
	AimInterpSpeed = 10.0f;
	AimSocketOffset = FVector(0.0f, 80.0f, 20.0f);
	HipSocketOffset = FVector(0.0f, 0.0f, 0.0f);
	RecoilAmount = 0.5f;
	RecoilTimer = 0.0f;
	CurrentRecoil = 0.0f;
	RecoilRecoverySpeed = 5.0f;
	RecoilKickSpeed = 10.0f;
	RecoilRecoveryDelay = 0.15f;

	// Movement Default Values
	AimWalkSpeed = 200.0f;
	BaseWalkSpeed = 500.0f;

	bIsAiming = false;

}

void AProjectAscentCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	WeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName);

	if (IsValid(WeaponComponent))
	{
		WeaponComponent->OnReloadStarted.AddDynamic(this, &AProjectAscentCharacter::HandleReloadStarted);
		WeaponComponent->OnFireStarted.AddDynamic(this, &AProjectAscentCharacter::HandleFireStarted);
	}
}

void AProjectAscentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurArmLength = CameraBoom->TargetArmLength;
	float TargetArmLength = (IsAiming()) ? AimArmLength : HipFireArmLength;

	float CurFOV = FollowCamera->FieldOfView;
	float TargetFOV = (IsAiming()) ? AimFOV : HipFireFOV;

	FVector CurSocketOffset = CameraBoom->SocketOffset;
	FVector TargetSocketOffset = (IsAiming()) ? AimSocketOffset : HipSocketOffset;

	CameraBoom->TargetArmLength = FMath::FInterpTo(CurArmLength, TargetArmLength, DeltaTime, AimInterpSpeed);
	FollowCamera->SetFieldOfView(FMath::FInterpTo(CurFOV, TargetFOV, DeltaTime, AimInterpSpeed));
	CameraBoom->SocketOffset = FMath::VInterpTo(CurSocketOffset, TargetSocketOffset, DeltaTime, AimInterpSpeed);
	
	// Move camera back down after recoil effects
	if (RecoilTimer > 0.0f)
	{
		RecoilTimer -= DeltaTime;
		float PreviousRecoil = CurrentRecoil;
		CurrentRecoil = FMath::FInterpTo(CurrentRecoil, TargetRecoil , DeltaTime, RecoilKickSpeed);
		AddControllerPitchInput(PreviousRecoil - CurrentRecoil);
	}
	else
	{
		float PreviousRecoil = CurrentRecoil;
		CurrentRecoil = FMath::FInterpTo(CurrentRecoil, 0, DeltaTime, RecoilRecoverySpeed);
		AddControllerPitchInput(PreviousRecoil - CurrentRecoil);
		TargetRecoil = 0.0f;
		RecoilTimer = 0.0f;
	}
}

  ////////////////////////////////////////////////////////////////////
 //							 Input                                 // 
////////////////////////////////////////////////////////////////////

void AProjectAscentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectAscentCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectAscentCharacter::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AProjectAscentCharacter::OnAimStarted);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AProjectAscentCharacter::OnAimEnded);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AProjectAscentCharacter::OnFire);
		
		// Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AProjectAscentCharacter::OnReload);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AProjectAscentCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProjectAscentCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProjectAscentCharacter::OnAimStarted(const FInputActionValue& Value)
{
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
}

void AProjectAscentCharacter::OnAimEnded(const FInputActionValue& Value)
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}


void AProjectAscentCharacter::OnFire(const FInputActionValue& Value)
{
	if (!IsValid(WeaponComponent) || !IsValid(FollowCamera))
	{
		return;
	}

	FVector TraceStart = FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (FollowCamera->GetForwardVector() * WeaponComponent->GetWeaponRange());
	WeaponComponent->Fire(TraceStart, TraceEnd);
}

void AProjectAscentCharacter::OnReload(const FInputActionValue& Value)
{
	if (!IsValid(WeaponComponent))
	{
		return;
	}

	WeaponComponent->Reload();
}

bool AProjectAscentCharacter::IsAiming() const
{
	return bIsAiming;
}


////////////////////////////////////////////////////////////////////
//							Delegate Handlers                     // 
////////////////////////////////////////////////////////////////////
void AProjectAscentCharacter::HandleReloadStarted()
{
	PlayAnimMontage(ReloadMontage);
}

void AProjectAscentCharacter::HandleFireStarted()
{
	TargetRecoil = CurrentRecoil + RecoilAmount;
	RecoilTimer = RecoilRecoveryDelay;
}