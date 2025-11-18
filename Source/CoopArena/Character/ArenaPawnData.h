// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArenaPawnData.generated.h"

class UArenaInputConfig;

/**
 * UArenaPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Arena Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class COOPARENA_API UArenaPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UArenaPawnData(const FObjectInitializer& ObjectInitializer);

public:
	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena|Input")
	TObjectPtr<UArenaInputConfig> InputConfig;
};
