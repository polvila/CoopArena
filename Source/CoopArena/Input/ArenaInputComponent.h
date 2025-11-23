// Copyright Pol Vilà. All Rights Reserved.

#pragma once

#include "ArenaInputConfig.h"
#include "EnhancedInputComponent.h"
#include "ArenaInputComponent.generated.h"


/**
 * UArenaInputComponent
 *
 *	Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class COOPARENA_API UArenaInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UArenaInputComponent();
	
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UArenaInputConfig* InputConfig, const FGameplayTag& InputTag,
		ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);
	
	void RemoveBinds(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UArenaInputComponent::BindNativeAction(const UArenaInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
