// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArenaGameInstance.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class COOPARENA_API UArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	virtual void Init() override;
};
