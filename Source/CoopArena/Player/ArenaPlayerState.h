// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ArenaPlayerState.generated.h"

class UArenaPawnData;

/**
 * AArenaPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class COOPARENA_API AArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	
	void SetPawnData(const UArenaPawnData* InPawnData);
	
	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface
	
protected:
	UFUNCTION()
	void OnRep_PawnData();
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UArenaPawnData> PawnData;
};
