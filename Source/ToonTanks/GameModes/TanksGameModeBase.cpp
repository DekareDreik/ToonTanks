// Fill out your copyright notice in the Description page of Project Settings.


#include "TanksGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/PlayerControllers/PlayerControllerBase.h"

void ATanksGameModeBase::BeginPlay() 
{
    Super::BeginPlay();
    //Get reference and game win/lose conditions.
    //Call HandleGameStart() to initialise the start countdown, turret activation, pawn check etc. 
    HandleGameStart();   
}

void ATanksGameModeBase::ActorDied(AActor* DeadActor) 
{
    if (DeadActor == PlayerTank)
    {
        PlayerTank->HandleDestruction();
        HandleGameOver(false);

        if (PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);
        }        
    }
    else if (APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))   
    {
        DestroyedTurret->HandleDestruction();
        if (--TargetTurrets == 0 )
        {
            HandleGameOver(true);
        }        
    }        
}

void ATanksGameModeBase::HandleGameStart() 
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));
    //Initialise then start countdown, turret cativation, pawn check etc.
    //Call Plueprints version GameStart();
    GameStart();
       if (PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);

            FTimerHandle PlayerEnableHandle;
            FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject (PlayerControllerRef, 
                &APlayerControllerBase::SetPlayerEnabledState, true);
            GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
        }    
}

void ATanksGameModeBase::HandleGameOver(bool PlayerWon) 
{
    //See if the Player has destroyed all the turrets, show win results.
    //else if turret destroyed player, show lose result.
    //Call blueprint version GameOver(boll).
    GameOver(PlayerWon);
}



int32 ATanksGameModeBase::GetTargetTurretCount() 
{
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticClass(), TurretActors);
    return TurretActors.Num();
}
