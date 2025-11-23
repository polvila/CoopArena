// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaGameInstance.h"

#include "ArenaGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"

void UArenaGameInstance::Init()
{
	Super::Init();
	
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(
			ArenaGameplayTags::InitState_Spawned,false,
			FGameplayTag());
		ComponentManager->RegisterInitState(
			ArenaGameplayTags::InitState_DataAvailable, false,
			ArenaGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(
			ArenaGameplayTags::InitState_DataInitialized, false,
			ArenaGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(
			ArenaGameplayTags::InitState_GameplayReady, false,
			ArenaGameplayTags::InitState_DataInitialized);
	}
}
