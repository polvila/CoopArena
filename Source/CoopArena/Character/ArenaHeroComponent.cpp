// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaHeroComponent.h"

#include "ArenaGameplayTags.h"
#include "ArenaLogChannels.h"
#include "ArenaPawnData.h"
#include "ArenaPawnExtensionComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/ArenaInputComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/ArenaPlayerController.h"
#include "Player/ArenaPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArenaHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

const FName UArenaHeroComponent::NAME_ActorFeatureName("Hero");

UArenaHeroComponent::UArenaHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UArenaHeroComponent::OnRegister()
{
	Super::OnRegister();
	
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogArena, Error, TEXT("[UArenaHeroComponent::OnRegister] This component has been added to a blueprint whose"
							   " base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("ArenaHeroComponent", "NotOnPawnError", "has been added to a blueprint"
							" whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
							" This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("ArenaHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UArenaHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == ArenaGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == ArenaGameplayTags::InitState_Spawned && 
		DesiredState == ArenaGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<AArenaPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			const AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = 
				Controller != nullptr &&
				Controller->PlayerState != nullptr &&
				Controller->PlayerState->GetOwner() == Controller;

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			const AArenaPlayerController* ArenaPC = GetController<AArenaPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !ArenaPC || !ArenaPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == ArenaGameplayTags::InitState_DataAvailable && 
		DesiredState == ArenaGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		const AArenaPlayerState* ArenaPS = GetPlayerState<AArenaPlayerState>();

		return ArenaPS && Manager->HasFeatureReachedInitState(Pawn, UArenaPawnExtensionComponent::NAME_ActorFeatureName,
			ArenaGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == ArenaGameplayTags::InitState_DataInitialized && 
		DesiredState == ArenaGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UArenaHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (CurrentState == ArenaGameplayTags::InitState_DataAvailable && 
		DesiredState == ArenaGameplayTags::InitState_DataInitialized)
	{
		const APawn* Pawn = GetPawn<APawn>();
		const AArenaPlayerState* ArenaPS = GetPlayerState<AArenaPlayerState>();
		if (!ensure(Pawn && ArenaPS))
		{
			return;
		}

		const AArenaPlayerController* ArenaPC = GetController<AArenaPlayerController>();
		if (ArenaPC && Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}
}

void UArenaHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UArenaPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == ArenaGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UArenaHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {ArenaGameplayTags::InitState_Spawned,
	                                                ArenaGameplayTags::InitState_DataAvailable,
	                                                ArenaGameplayTags::InitState_DataInitialized,
	                                                ArenaGameplayTags::InitState_GameplayReady};

	// This will try to progress from spawned (which is only set in BeginPlay) 
	// through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UArenaHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UArenaPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(ArenaGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UArenaHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UArenaHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = 
		PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	
	if (const UArenaPawnExtensionComponent* PawnExtComp = UArenaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UArenaPawnData* PawnData = PawnExtComp->GetPawnData<UArenaPawnData>())
		{
			if (const UArenaInputConfig* InputConfig = PawnData->InputConfig)
			{
				if (UInputMappingContext* IMC = InputMapping.LoadSynchronous())
				{
					if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
					{
						Settings->RegisterInputMappingContext(IMC);
					}
					
					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;
					// Actually add the config to the local player
					Subsystem->AddMappingContext(IMC, 0, Options);
				}
				
				UArenaInputComponent* ArenaIC = Cast<UArenaInputComponent>(PlayerInputComponent);
				if (ensureMsgf(ArenaIC, TEXT("Unexpected Input Component class! "
								 "Change the input component to UArenaInputComponent or a subclass of it.")))
				{
					ArenaIC->BindNativeAction(InputConfig, ArenaGameplayTags::InputTag_Move, ETriggerEvent::Triggered,
						this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}
}

void UArenaHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	if (Controller)	
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

