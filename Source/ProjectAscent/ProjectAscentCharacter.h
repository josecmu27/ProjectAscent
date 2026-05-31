// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "HealthComponent.h"
#include "WeaponComponent.h"
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

    /* Components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWeaponComponent* WeaponComponent;

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

    /* ADS Settings*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float HipFireArmLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float AimArmLength;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float RecoilAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float CurrentRecoil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float RecoilRecoverySpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float RecoilKickSpeed;

    float RecoilTimer;

    float TargetRecoil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS Settings", meta = (AllowPrivateAccess = "true"))
    float RecoilRecoveryDelay;

    /* Movement Settings*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float AimWalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float BaseWalkSpeed;

    /* Weapon Settings*/

    /* Animation */
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* ReloadMontage;

    /* States */
    bool bIsAiming;

    /* Delegate Handlers*/
    UFUNCTION()
    void HandleReloadStarted();

    UFUNCTION()
    void HandleFireStarted();

};

