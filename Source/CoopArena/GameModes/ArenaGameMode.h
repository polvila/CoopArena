// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

class UArenaPawnData;

/**
 * AArenaGameMode
 *
 *	The base game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class COOPARENA_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UArenaPawnData* GetPawnDataForController(const AController* InController) const;

	//~AGameModeBase interface
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	//~End of AGameModeBase interface
	
public:
	UPROPERTY(EditDefaultsOnly, Category= "Lyra|Pawn")
	TObjectPtr<const UArenaPawnData> DefaultPawnData;
};
