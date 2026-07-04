// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "HealthComponent.h"
#include "WeaponComponent.h"
#include "InventoryComponent.h"
#include "GrapplingHookComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProjectAscentCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProjectAscentCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    AProjectAscentCharacter();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure, Category = "ADS")
    bool IsAiming() const;

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void OnWeaponFired(UAnimMontage* FireMontage, USoundBase* FireSound);

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void OnWeaponReloaded(UAnimMontage* ReloadMontage, USoundBase* ReloadSound);

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void OnWeaponHit(const FHitResult& HitResult);

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void OnActiveWeaponChanged();

    UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /* Input Hanlders */
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void OnAimStarted(const FInputActionValue& Value);
    void OnAimEnded(const FInputActionValue& Value);
    void OnFire(const FInputActionValue& Value);
    void OnReload(const FInputActionValue& Value);
    void OnGrapple(const FInputActionValue& Value);
    void OnUngrapple(const FInputActionValue& Value);
    void OnCrouchStarted(const FInputActionValue& Value);
    void OnCrouchEnded(const FInputActionValue& Value);
    void OnSwitchWeapon(const FInputActionValue& Value);

    /* Components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UGrapplingHookComponent* GrapplingHookComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    UInventoryComponent* InventoryComponent;

    /* Inventory Components*/

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* HeldWeaponMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BackSlotOneMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BackSlotTwoMeshComponent;

    /* Weapon Settings*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
    UWeaponDataAsset* DefaultWeaponOneData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
    UWeaponDataAsset* DefaultWeaponTwoData;


    UFUNCTION(BlueprintCallable, Category = "Weapons")
    void UpdateWeaponMeshes();

private:

    /* Camera Components*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    /* Input Settings*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* FireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* ReloadAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* GrappleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* UngrappleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* SwitchWeaponAction;

    /* ADS Settings*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float HipFireArmLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float AimArmLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float CrouchArmOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float StandArmOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float HipFireFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float AimFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float AimInterpSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    FVector AimSocketOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    FVector HipSocketOffset;

    float CurrentRecoil;

    float RecoilTimer;

    float TargetRecoil;

    /* Movement Settings*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float AimWalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float BaseWalkSpeed;

    /* States */
    bool bIsAiming;

    /* Delegate Handlers*/
    UFUNCTION()
    void HandleReloadStarted(EWeaponType WeaponType);

    UFUNCTION()
    void HandleFireStarted(EWeaponType WeaponType);

    UFUNCTION()
    void HandleActiveWeaponChanged();

    UFUNCTION()
    void HandleWeaponHit(FHitResult HitResult);

    UFUNCTION()
    void HandleNewWeaponEquipped(UWeaponComponent* NewWeapon, EWeaponSlot Slot);
};

