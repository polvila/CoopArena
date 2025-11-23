// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArenaInputComponent)

UArenaInputComponent::UArenaInputComponent()
{
}

void UArenaInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (const uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}


