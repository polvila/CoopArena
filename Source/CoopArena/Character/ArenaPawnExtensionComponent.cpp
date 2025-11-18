// Copyright Pol Vilà. All Rights Reserved.


#include "ArenaPawnExtensionComponent.h"

#include "ArenaGameplayTags.h"
#include "ArenaLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "ArenaPawnData.h"
#include "Components/GameFrameworkComponentManager.h"

const FName UArenaPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UArenaPawnExtensionComponent::UArenaPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
}

void UArenaPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArenaPawnExtensionComponent, PawnData);
}

void UArenaPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("ArenaPawnExtensionComponent on [%s] can only be added to Pawn actors."),
		*GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf(PawnExtensionComponents.Num() == 1, TEXT("Only one ArenaPawnExtensionComponent should exist on [%s]."), 
		*GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UArenaPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	// Notifies state manager that we have spawned
	ensure(TryToChangeInitState(ArenaGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UArenaPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UArenaPawnExtensionComponent::SetPawnData(const UArenaPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogArena, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."),
			*GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UArenaPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void UArenaPawnExtensionComponent::HandleControllerChanged()
{
	CheckDefaultInitialization();
}

void UArenaPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UArenaPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UArenaPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = {
		ArenaGameplayTags::InitState_Spawned,
		ArenaGameplayTags::InitState_DataAvailable,
		ArenaGameplayTags::InitState_DataInitialized,
		ArenaGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages
	// until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

bool UArenaPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == ArenaGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == ArenaGameplayTags::InitState_Spawned && DesiredState == ArenaGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == ArenaGameplayTags::InitState_DataAvailable && DesiredState == ArenaGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, ArenaGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == ArenaGameplayTags::InitState_DataInitialized && DesiredState == ArenaGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UArenaPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == ArenaGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UArenaPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == ArenaGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}


