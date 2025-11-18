// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaCharacter.h"


AArenaCharacter::AArenaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

