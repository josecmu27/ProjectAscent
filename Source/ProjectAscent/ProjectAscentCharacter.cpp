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
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
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
	GrapplingHookComponent = CreateDefaultSubobject<UGrapplingHookComponent>(TEXT("GrapplingHookComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	
	HeldWeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldWeaponMeshComponent"));
	BackSlotOneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackSlotOneMeshComponent"));
	BackSlotTwoMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackSlotTwoMeshComponent"));


	// ADS Default Values
	HipFireArmLength = 400.0f;
	AimArmLength = 200.0f;
	CrouchArmOffset = -40.0f;
	StandArmOffset = 0.0f;

	HipFireFOV = 90.0f;
	AimFOV = 60.0f;

	AimInterpSpeed = 10.0f;
	AimSocketOffset = FVector(0.0f, 80.0f, 20.0f);
	HipSocketOffset = FVector(0.0f, 0.0f, 0.0f);
	RecoilTimer = 0.0f;
	CurrentRecoil = 0.0f;

	// Movement Default Values
	AimWalkSpeed = 200.0f;
	BaseWalkSpeed = 500.0f;
	bIsAiming = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(48.0f);
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

void AProjectAscentCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (IsValid(HeldWeaponMeshComponent) && IsValid(BackSlotOneMeshComponent) && IsValid(BackSlotTwoMeshComponent))
	{
		HeldWeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Weapon_Socket"));
		BackSlotOneMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BackSlot_01"));
		BackSlotTwoMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BackSlot_02"));
	}

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->EquipNewWeapon(DefaultWeaponOneData, EWeaponSlot::SlotOne);
		InventoryComponent->EquipNewWeapon(DefaultWeaponTwoData, EWeaponSlot::SlotTwo);

		InventoryComponent->OnActiveWeaponFire.AddDynamic(this, &AProjectAscentCharacter::HandleFireStarted);
		InventoryComponent->OnActiveWeaponReloadStarted.AddDynamic(this, &AProjectAscentCharacter::HandleReloadStarted);
		InventoryComponent->OnActiveWeaponReloadCycle.AddDynamic(this, &AProjectAscentCharacter::HandleReloadCycle);
		InventoryComponent->OnActiveWeaponReloadEnded.AddDynamic(this, &AProjectAscentCharacter::HandleReloadEnded);
		InventoryComponent->OnActiveWeaponChanged.AddDynamic(this, &AProjectAscentCharacter::HandleActiveWeaponChanged);
		InventoryComponent->OnNewWeaponEquipped.AddDynamic(this, &AProjectAscentCharacter::HandleNewWeaponEquipped);


		UWeaponComponent* WeaponOne = InventoryComponent->GetWeaponAtSlot(EWeaponSlot::SlotOne);
		UWeaponComponent* WeaponTwo = InventoryComponent->GetWeaponAtSlot(EWeaponSlot::SlotTwo);

		if (IsValid(WeaponOne)) WeaponOne->OnWeaponHit.AddDynamic(this, &AProjectAscentCharacter::HandleWeaponHit);
		if (IsValid(WeaponTwo)) WeaponTwo->OnWeaponHit.AddDynamic(this, &AProjectAscentCharacter::HandleWeaponHit);
	}

	UpdateWeaponMeshes();

}

void AProjectAscentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurArmLength = CameraBoom->TargetArmLength;
	float TargetArmLength = (IsAiming()) ? AimArmLength : HipFireArmLength;
	float TargetSocketOffsetZ = (GetCharacterMovement() && GetCharacterMovement()->IsCrouching()) ? CrouchArmOffset : StandArmOffset;

	float CurFOV = FollowCamera->FieldOfView;
	float TargetFOV = (IsAiming()) ? AimFOV : HipFireFOV;

	FVector CurSocketOffset = CameraBoom->SocketOffset;
	FVector TargetSocketOffset = (IsAiming()) ? AimSocketOffset : HipSocketOffset;
	TargetSocketOffset.Z += TargetSocketOffsetZ;

	CameraBoom->TargetArmLength = FMath::FInterpTo(CurArmLength, TargetArmLength, DeltaTime, AimInterpSpeed);
	FollowCamera->SetFieldOfView(FMath::FInterpTo(CurFOV, TargetFOV, DeltaTime, AimInterpSpeed));
	CameraBoom->SocketOffset = FMath::VInterpTo(CurSocketOffset, TargetSocketOffset, DeltaTime, AimInterpSpeed);

	if (!IsValid(InventoryComponent)) return;

	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();
	if (!IsValid(ActiveWeapon) || !IsValid(ActiveWeapon->GetWeaponData())) return;
	
	// Move camera back down after recoil effects
	if (RecoilTimer > 0.0f)
	{
		RecoilTimer -= DeltaTime;
		float PreviousRecoil = CurrentRecoil;
		CurrentRecoil = FMath::FInterpTo(CurrentRecoil, TargetRecoil , DeltaTime, ActiveWeapon->GetWeaponData()->RecoilKickSpeed);
		AddControllerPitchInput(PreviousRecoil - CurrentRecoil);
	}
	else
	{
		float PreviousRecoil = CurrentRecoil;
		CurrentRecoil = FMath::FInterpTo(CurrentRecoil, 0, DeltaTime, ActiveWeapon->GetWeaponData()->RecoilRecoverySpeed);
		AddControllerPitchInput(PreviousRecoil - CurrentRecoil);
		TargetRecoil = 0.0f;
		RecoilTimer = 0.0f;
	}
}

/**
 * @brief Synchronizes all three weapon mesh components with the current inventory state.
 *        Called whenever the active weapon changes (OnActiveWeaponChanged) or a new
 *        weapon is equipped (OnNewWeaponEquipped) to update visual representation
 *		  based on inventory data.
 *
 *        Three mesh components are updated independently:
 *        - HeldWeaponMeshComponent:  shows the active weapon in the player's hands
 *        - BackSlotOneMeshComponent: shows SlotOne's weapon on the back (empty if active)
 *        - BackSlotTwoMeshComponent: shows SlotTwo's weapon on the back (empty if active)
 */
void AProjectAscentCharacter::UpdateWeaponMeshes()
{
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();

	if (IsValid(ActiveWeapon) && IsValid(ActiveWeapon->GetWeaponData()))
	{
		HeldWeaponMeshComponent->SetStaticMesh(ActiveWeapon->GetWeaponData()->WeaponMesh);
	}
	else
	{
		HeldWeaponMeshComponent->SetStaticMesh(nullptr);
	}

	UWeaponComponent* WeaponOne = InventoryComponent->GetWeaponAtSlot(EWeaponSlot::SlotOne);
	UWeaponComponent* WeaponTwo = InventoryComponent->GetWeaponAtSlot(EWeaponSlot::SlotTwo);

	auto GetBackSlotMesh = [&](UWeaponComponent* Weapon) -> UStaticMesh*
		{
			if (Weapon != ActiveWeapon && IsValid(Weapon) && IsValid(Weapon->GetWeaponData()))
				return Weapon->GetWeaponData()->WeaponMesh;
			return nullptr;
		};

	BackSlotOneMeshComponent->SetStaticMesh(GetBackSlotMesh(WeaponOne));
	BackSlotTwoMeshComponent->SetStaticMesh(GetBackSlotMesh(WeaponTwo));
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

		// Grappling Hook
		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Started, this, &AProjectAscentCharacter::OnGrapple);
		EnhancedInputComponent->BindAction(UngrappleAction, ETriggerEvent::Started, this, &AProjectAscentCharacter::OnUngrapple);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AProjectAscentCharacter::OnCrouchStarted);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AProjectAscentCharacter::OnCrouchEnded);

		// Switch Weapon
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &AProjectAscentCharacter::OnSwitchWeapon);
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
	if (!IsValid(InventoryComponent)) return;
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();

	if (!IsValid(ActiveWeapon) || !IsValid(FollowCamera)) return;

	FVector TraceStart = FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (FollowCamera->GetForwardVector() * ActiveWeapon->GetWeaponRange());
	ActiveWeapon->Fire(TraceStart, TraceEnd);
}

void AProjectAscentCharacter::OnReload(const FInputActionValue& Value)
{
	if (!IsValid(InventoryComponent)) return;
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();

	if (!IsValid(ActiveWeapon)) return;

	ActiveWeapon->Reload();
}

void AProjectAscentCharacter::OnGrapple(const FInputActionValue& Value)
{
	if (!IsValid(GrapplingHookComponent)) return;

	FVector TraceStart = FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (FollowCamera->GetForwardVector() * GrapplingHookComponent->GetGrapplingHookRange());
	GrapplingHookComponent->FireHook(TraceStart, TraceEnd);
}

void AProjectAscentCharacter::OnUngrapple(const FInputActionValue& Value)
{ 
	if (!IsValid(GrapplingHookComponent)) return;

	GrapplingHookComponent->RetractHook();
}

void AProjectAscentCharacter::OnCrouchStarted(const FInputActionValue& Value)
{
	Crouch();
}

void AProjectAscentCharacter::OnCrouchEnded(const FInputActionValue& Value)
{
	UnCrouch();
}

void AProjectAscentCharacter::OnSwitchWeapon(const FInputActionValue& Value)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->SwitchWeapon();
	}
}

bool AProjectAscentCharacter::IsAiming() const
{
	return bIsAiming;
}


////////////////////////////////////////////////////////////////////
//							Delegate Handlers                     // 
////////////////////////////////////////////////////////////////////

void AProjectAscentCharacter::HandleReloadStarted(EWeaponType WeaponType)
{
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();
	if (!IsValid(ActiveWeapon) || !IsValid(ActiveWeapon->GetWeaponData())) return;

	OnWeaponReloadStarted(ActiveWeapon->GetWeaponData()->ReloadStartMontage, ActiveWeapon->GetWeaponData()->ReloadSound);
}

void AProjectAscentCharacter::HandleReloadEnded(EWeaponType WeaponType)
{
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();
	if (!IsValid(ActiveWeapon) || !IsValid(ActiveWeapon->GetWeaponData())) return;

	OnWeaponReloadEnded(ActiveWeapon->GetWeaponData()->ReloadEndMontage);
}

void AProjectAscentCharacter::HandleFireStarted(EWeaponType WeaponType)
{
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();
	if (!IsValid(ActiveWeapon) || !IsValid(ActiveWeapon->GetWeaponData())) return;

	UWeaponDataAsset* WeaponData = ActiveWeapon->GetWeaponData();
	TargetRecoil = CurrentRecoil + WeaponData->RecoilAmount;
	RecoilTimer = WeaponData->RecoilRecoveryDelay;

	OnWeaponFired(WeaponData->FireMontage, WeaponData->FireSound);
}

void AProjectAscentCharacter::HandleActiveWeaponChanged()
{
	UpdateWeaponMeshes();
	OnActiveWeaponChanged();
}

void AProjectAscentCharacter::HandleWeaponHit(FHitResult HitResult)
{
	OnWeaponHit(HitResult);
}

void AProjectAscentCharacter::HandleNewWeaponEquipped(UWeaponComponent* NewWeapon, EWeaponSlot Slot)
{
	if (IsValid(NewWeapon))
	{
		NewWeapon->OnWeaponHit.AddDynamic(this, &AProjectAscentCharacter::HandleWeaponHit);
	}

	UpdateWeaponMeshes();
}

void AProjectAscentCharacter::HandleReloadCycle(EWeaponType WeaponType)
{
	UWeaponComponent* ActiveWeapon = InventoryComponent->GetActiveWeapon();
	if (!IsValid(ActiveWeapon) || !IsValid(ActiveWeapon->GetWeaponData())) return;
	OnWeaponReloadCycle(ActiveWeapon->GetWeaponData()->ReloadCycleMontage, ActiveWeapon->GetWeaponData()->ReloadSound);
}