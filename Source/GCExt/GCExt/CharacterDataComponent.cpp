// Copyright (C) 2023 owoDra

#include "CharacterDataComponent.h"

#include "CharacterData.h"
#include "CharacterModifier.h"
#include "CharacterModifierInstance.h"
#include "GCExtLogs.h"

#include "InitStateTags.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterDataComponent)


UCharacterDataComponent::UCharacterDataComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ModifierContainer(this)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UCharacterDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ModifierContainer);
	DOREPLIFETIME(ThisClass, CharacterData);
}


void UCharacterDataComponent::OnRegister()
{
	Super::OnRegister();

	// This component can only be added to classes derived from APawn

	const auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("BEPawnBasicComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Pawn.

	TArray<UActorComponent*> Components;
	Pawn->GetComponents(UCharacterDataComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one BEPawnBasicComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UCharacterDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start listening for changes in the initialization state of all features 
	// related to the Pawn that owns this component.

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Change the initialization state of this component to [Spawned]

	ensure(TryToChangeInitState(TAG_InitState_Spawned));

	// Check if initialization process can continue

	CheckDefaultInitialization();
}

void UCharacterDataComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


bool UCharacterDataComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	auto* Pawn{ GetPawn<APawn>() };

	/**
	 * [None] -> [Spawned]
	 * 
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		// Check Owning Pawn

		if (Pawn)
		{
			return true;
		}
	}

	/**
	 * [Spawned] -> [DataAvailable]
	 * 
	 * Tips:
	 *	At any time that the current state is valid (Spawned or later), 
	 *	an attempt can be made to move to this state.
	 * 
	 *  When CharacterData is changed or a new feature is added to Character later,
	 *	it is moved to this state to reset the initialization state.
	 */
	if (CurrentState.IsValid() && DesiredState == TAG_InitState_DataAvailable)
	{
		// Check CharacterData
		// CharacterData is always required for character initialization

		if (CharacterData)
		{
			return true;
		}
	}

	/**
	 * [DataAvailable] -> [DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		// Whether all other functions of Pawn that own this component have reached [DataInitialized]

		return Manager->HaveAllFeaturesReachedInitState(Pawn, TAG_InitState_DataInitialized, NAME_ActorFeatureName);
	}

	/**
	 * [DataInitialized] -> [GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		// Whether all other functions of Pawn that own this component have reached [GameplayReady]

		return Manager->HaveAllFeaturesReachedInitState(Pawn, TAG_InitState_GameplayReady, NAME_ActorFeatureName);
	}

	return false;
}

void UCharacterDataComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	/**
	 * [DataAvailable] -> [DataInitialized]
	 */
	if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		OnCharacterDataInitializedDelegate.Broadcast();
	}
}

void UCharacterDataComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// Executed when the initialization state is changed 
	// by a feature other than this component of the Pawn that owns this component.

	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		/**
		 * -> [Spawned]
		 * 
		 * Tips:
		 *	Rewind the initialization state of this component to [DataAvailable] 
		 *	when other functions reach [Spawned]
		 * 
		 *  To be able to understand the initialization state of 
		 *	the entire Pawn features through this component.
		 */
		if (Params.FeatureState == TAG_InitState_Spawned)
		{
			//(TryToChangeInitState(TAG_InitState_DataAvailable));

			//CheckDefaultInitialization();
		}

		/**
		 * -> [DataInitialized]
		 */
		else if (Params.FeatureState == TAG_InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}

		/**
		 * -> [GameplayReady]
		 */
		else if (Params.FeatureState == TAG_InitState_GameplayReady)
		{
			CheckDefaultInitialization();
		}
	}
}

void UCharacterDataComponent::CheckDefaultInitialization()
{
	// Perform initialization state checks on other features before checking the initialization state of this component

	CheckDefaultInitializationForImplementers();

	// @TODO: Allow changes from DeveloperSetting

	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}


bool UCharacterDataComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	auto bWroteSomething{ Super::ReplicateSubobjects(Channel, Bunch, RepFlags) };

	for (const auto& Entry : ModifierContainer.Entries)
	{
		const auto& Instance{ Entry.Instance };

		if (Instance)
		{
			bWroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UCharacterDataComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const auto& Entry : ModifierContainer.Entries)
		{
			const auto& Instance{ Entry.Instance };

			if (Instance)
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}


void UCharacterDataComponent::OnRep_CharacterData(const UCharacterData* OldData)
{
	UE_LOG(LogGCE, Log, TEXT("OldCharacterData: %s"), *GetNameSafe(OldData));

	UpdateCharacterData();
}

void UCharacterDataComponent::UpdateCharacterData()
{
	check(CharacterData);

	auto* Pawn{ GetPawnChecked<APawn>() };

	if (Pawn->HasAuthority())
	{
		if (!ModifierContainer.Entries.IsEmpty())
		{
			ModifierContainer.RemoveAllEntries(Pawn);
		}

		auto ModifiersToApply{ TArray<UCharacterModifier*>() };
		CharacterData->GetModifiers(ModifiersToApply);
		for (const auto& Modifier : ModifiersToApply)
		{
			ModifierContainer.AddEntry(Modifier, Pawn);
		}
	}

	ensure(TryToChangeInitState(TAG_InitState_DataAvailable));

	CheckDefaultInitialization();
}

void UCharacterDataComponent::SetCharacterData(const UCharacterData* NewCharacterData)
{
	auto* Pawn{ GetPawnChecked<APawn>() };

	if (Pawn->HasAuthority())
	{
		if (NewCharacterData != CharacterData)
		{
			CharacterData = NewCharacterData;

			UpdateCharacterData();
		}
	}
}


void UCharacterDataComponent::OnCharacterDataInitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnCharacterDataInitializedDelegate.IsBoundToObject(Delegate.GetUObject()))
	{
		OnCharacterDataInitializedDelegate.Add(Delegate);
	}
}
