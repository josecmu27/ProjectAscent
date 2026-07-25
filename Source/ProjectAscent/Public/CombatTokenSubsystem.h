// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatRoleSubsystem.h"
#include "EnemyBase.h"
#include "CombatTokenSettings.h"
#include "CombatTokenSubsystem.generated.h"


USTRUCT()
struct FTokenBank
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    int32 TokenBudget = 0;

    UPROPERTY(VisibleAnywhere)
    TMap<AEnemyBase*, float> Holders; // holder -> time the token was last granted

    UPROPERTY(VisibleAnywhere)
    TMap<AEnemyBase*, float> Requesters; // requester -> time the request was made

};

USTRUCT()
struct FScoredEnemy
{
    GENERATED_BODY()

    AEnemyBase* Enemy = nullptr;
    
    float Score = 0.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTASCENT_API UCombatTokenSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
public:

	void RequestToken(AEnemyBase* Enemy);

    void ReturnToken(AEnemyBase* Enemy);

    void CancelRequest(AEnemyBase* Enemy);

    const TMap<ECombatRole, FTokenBank>& GetTokenBanks() const { return TokenBanks; }

private:

    FScoredEnemy FindBestRequester(const FTokenBank& TokenBank) const;

    FScoredEnemy FindWeakestHolder(const FTokenBank& TokenBank) const;

    TMap<ECombatRole, FTokenBank> TokenBanks;

    const UCombatTokenSettings* Settings;

};
