// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatTokenSubsystem.h"



void UCombatTokenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    // Populates one FTokenBank per combat role at world startup.
    // TokenBudget is currently hardcoded; will read from the Director once it exists

    Super::Initialize(Collection);

    const TArray<ECombatRole> AllCombatRoles = { ECombatRole::Engager, ECombatRole::Defender };

    Settings = GetDefault<UCombatTokenSettings>();

    for (ECombatRole CombatRole : AllCombatRoles)
    {
        FTokenBank NewBank;
        NewBank.TokenBudget = 2; // TODO: replace with Director value
        TokenBanks.Add(CombatRole, NewBank);
    }

}

/**
 * @brief Processes a token request for an enemy's combat role bank.
 *        Grants the token to the best scoring requester if the bank is within token budget, 
 *        otherwise attempts to steal from the worst scoring current holder.
 *        
 * @param Enemy The enemy that requested a token. Its current combat role
 *              determines which bank the request is processed against.
 */
void UCombatTokenSubsystem::RequestToken(AEnemyBase* Enemy)
{
    if (!IsValid(Enemy)) return;

    if (!TokenBanks.Contains(Enemy->GetCombatRole())) return;

    // Add Requester to their respective token bank
    FTokenBank& TokenBank = TokenBanks[Enemy->GetCombatRole()];

    if (!TokenBank.Requesters.Contains(Enemy))
    {
        TokenBank.Requesters.Add(Enemy, GetWorld()->GetTimeSeconds());
    }

    // Check if within budget
    if (TokenBank.Holders.Num() < TokenBank.TokenBudget)
    {
        // Grant to best requester
        FScoredEnemy BestRequester = FindBestRequester(TokenBank);
        if (!BestRequester.Enemy) return;

        // Update Bank
        TokenBank.Requesters.Remove(BestRequester.Enemy);
        TokenBank.Holders.Add(BestRequester.Enemy, GetWorld()->GetTimeSeconds());
        BestRequester.Enemy->AcceptToken();
        return;
    }

    // Check if any requester can steal

    FScoredEnemy BestRequester = FindBestRequester(TokenBank);
    if (!BestRequester.Enemy) return;

    FScoredEnemy WeakestHolder = FindWeakestHolder(TokenBank);
    if (!WeakestHolder.Enemy) return;

    // Don't steal if score difference isn't significant
    if (BestRequester.Score - WeakestHolder.Score < Settings->MinStealMargin ) return; 


    // Update Bank
    TokenBank.Holders.Remove(WeakestHolder.Enemy);
    WeakestHolder.Enemy->SurrenderToken();

    TokenBank.Requesters.Remove(BestRequester.Enemy);
    TokenBank.Holders.Add(BestRequester.Enemy, GetWorld()->GetTimeSeconds());
    BestRequester.Enemy->AcceptToken();

}

/**
 * @brief Processes a token return for an enemy's combat role bank.
 *        Clears Enemy's combat token and fills in emptied slot by granting 
 *        a combat token to the best scoring requester. 
 *
 * @param[in] Enemy The enemy that returned the token. Its current combat role
 *                  determines which bank the return is processed against.
 */
void UCombatTokenSubsystem::ReturnToken(AEnemyBase* Enemy)
{
    FTokenBank& TokenBank = TokenBanks[Enemy->GetCombatRole()];

    if (!TokenBank.Holders.Contains(Enemy)) return;

    TokenBank.Holders.Remove(Enemy);
    Enemy->ClearToken();
    
    // Fill in emptied slot with best requester
    FScoredEnemy BestRequester = FindBestRequester(TokenBank);
    if (!BestRequester.Enemy) return;


    // Update Bank
    TokenBank.Requesters.Remove(BestRequester.Enemy);
    TokenBank.Holders.Add(BestRequester.Enemy, GetWorld()->GetTimeSeconds());
    BestRequester.Enemy->AcceptToken();
   

}

/**
 * @brief Processes an enemy's token request cancellation for an enemy's combat role bank.
 *        Removes the enemy from the requesters map of the combat role bank. 
 * 
 * @param[in] Enemy The enemy that canceled their request. Its current combat role
 *                  determines which bank to remove the enemy requester.
 */
void UCombatTokenSubsystem::CancelRequest(AEnemyBase* Enemy)
{
    FTokenBank& TokenBank = TokenBanks[Enemy->GetCombatRole()];

    if (!TokenBank.Requesters.Contains(Enemy)) return;

    // Update Bank
    TokenBank.Requesters.Remove(Enemy);
    Enemy->ClearToken();
}

/* ----------------------------PRIVATE FUNCTIONS---------------------------------------*/

/**
 * @brief Find the best scoring requester in TokenBank's requesters map.
 *
 * @param[in] TokenBank The bank to find the best scoring requester
 */
FScoredEnemy UCombatTokenSubsystem::FindBestRequester(const FTokenBank& TokenBank) const
{
    AEnemyBase* BestRequester = nullptr;
    float BestScore = TNumericLimits<float>::Lowest();

    for (const TPair<AEnemyBase*, float>& Requester : TokenBank.Requesters)
    {
        float WaitTime = GetWorld()->GetTimeSeconds() - Requester.Value;
        float Score = Requester.Key->GetTokenPriorityScore(WaitTime);
        if (Score > BestScore)
        {
            BestScore = Score;
            BestRequester = Requester.Key;
        }
    }

    return FScoredEnemy(BestRequester, BestScore);
}

/**
 * @brief Find the weakest scoring token holder in TokenBank's holders map.
 *
 * @param[in] TokenBank The bank to find the weakest scoring token holder
 */
FScoredEnemy UCombatTokenSubsystem::FindWeakestHolder(const FTokenBank& TokenBank) const
{
    AEnemyBase* WeakestHolder = nullptr;
    float WeakestScore = TNumericLimits<float>::Max();

    for (const TPair<AEnemyBase*, float>& Holder : TokenBank.Holders)
    {
        if (GetWorld()->GetTimeSeconds() - Holder.Value < Settings->StealCooldownDuration) continue;
        float Score = Holder.Key->GetTokenPriorityScore(0.0f); // holders get no wait bonus
        if (Score < WeakestScore)
        {
            WeakestScore = Score;
            WeakestHolder = Holder.Key;
        }
    }

    return FScoredEnemy(WeakestHolder, WeakestScore);
}
