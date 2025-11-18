// Copyright Pol Vilà. All Rights Reserved.

#include "ArenaInputConfig.h"

#include "ArenaLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArenaInputConfig)

const UInputAction* UArenaInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag,
	const bool bLogNotFound) const
{
	for (const FArenaInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogArena, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."),
			*InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
