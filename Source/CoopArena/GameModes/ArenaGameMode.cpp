// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaGameMode.h"

#include "ArenaGameState.h"
#include "Character/ArenaCharacter.h"
#include "Player/ArenaPlayerController.h"
#include "Player/ArenaPlayerState.h"
#include "UI/ArenaHUD.h"


AArenaGameMode::AArenaGameMode()
{
	GameStateClass = AArenaGameState::StaticClass();
	PlayerControllerClass = AArenaPlayerController::StaticClass();
	PlayerStateClass = AArenaPlayerState::StaticClass();
	DefaultPawnClass = AArenaCharacter::StaticClass();
	HUDClass = AArenaHUD::StaticClass();
}
