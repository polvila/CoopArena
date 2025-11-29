// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/ModularExperienceCharacter.h"
#include "ArenaCharacter.generated.h"

UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class COOPARENA_API AArenaCharacter : public AModularExperienceCharacter
{
	GENERATED_BODY()

public:
	AArenaCharacter();

};
