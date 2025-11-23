// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaGameMode.h"

#include "ArenaGameState.h"
#include "ArenaLogChannels.h"
#include "Character/ArenaCharacter.h"
#include "Character/ArenaPawnData.h"
#include "Character/ArenaPawnExtensionComponent.h"
#include "Player/ArenaPlayerController.h"
#include "Player/ArenaPlayerState.h"
#include "UI/ArenaHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArenaGameMode)

AArenaGameMode::AArenaGameMode()
{
	GameStateClass = AArenaGameState::StaticClass();
	PlayerControllerClass = AArenaPlayerController::StaticClass();
	PlayerStateClass = AArenaPlayerState::StaticClass();
	DefaultPawnClass = AArenaCharacter::StaticClass();
	HUDClass = AArenaHUD::StaticClass();
}

const UArenaPawnData* AArenaGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const AArenaPlayerState* LyraPS = InController->GetPlayerState<AArenaPlayerState>())
		{
			if (const UArenaPawnData* PawnData = LyraPS->GetPawnData<UArenaPawnData>())
			{
				return PawnData;
			}
		}
	}
	
	return DefaultPawnData;
}

UClass* AArenaGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UArenaPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AArenaGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UArenaPawnExtensionComponent* PawnExtComp = 
				UArenaPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UArenaPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogArena, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."),
						*GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogArena, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."),
				*GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogArena, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}
