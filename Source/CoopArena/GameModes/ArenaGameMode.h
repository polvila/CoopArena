// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/ModularExperienceGameMode.h"
#include "ArenaGameMode.generated.h"

class UArenaPawnData;

/**
 * AArenaGameMode
 *
 *	The base game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class COOPARENA_API AArenaGameMode : public AModularExperienceGameMode
{
	GENERATED_BODY()

public:
	AArenaGameMode();
	
};
