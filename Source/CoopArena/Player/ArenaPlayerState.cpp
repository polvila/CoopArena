// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaPlayerState.h"

#include "ArenaLogChannels.h"
#include "Character/ArenaPawnData.h"
#include "GameModes/ArenaGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArenaPlayerState)

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}

void AArenaPlayerState::SetPawnData(const UArenaPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogArena, Error,
			TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."),
			*GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	ForceNetUpdate();
}

void AArenaPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AArenaPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		if (AArenaGameMode* ArenaGameMode = GetWorld()->GetAuthGameMode<AArenaGameMode>())
		{
			if (const UArenaPawnData* NewPawnData = ArenaGameMode->GetPawnDataForController(GetOwningController()))
			{
				SetPawnData(NewPawnData);
			}
			else
			{
				UE_LOG(LogArena, Error, 
					TEXT("AArenaPlayerState::PostInitializeComponents(): Unable to find PawnData to initialize player state [%s]!"), 
					*GetNameSafe(this));
			}
		}
	}
}

void AArenaPlayerState::OnRep_PawnData()
{
}
