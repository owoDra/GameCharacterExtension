// Copyright (C) 2023 owoDra

#include "CharacterDataComponent.h"

#include "CharacterData.h"
#include "CharacterModifier.h"
#include "GCExtLogs.h"

#include "InitState/InitStateTags.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterDataComponent)


UCharacterDataComponent::UCharacterDataComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UCharacterDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterData);
}


void UCharacterDataComponent::OnRegister()
{
	Super::OnRegister();

	// This component can only be added to classes derived from APawn

	const auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("CharacterDataComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Pawn.

	TArray<UActorComponent*> Components;
	Pawn->GetComponents(UCharacterDataComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one CharacterDataComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

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
	 */
	if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
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
		 * -> [DataInitialized]
		 */
		if (Params.FeatureState == TAG_InitState_DataInitialized)
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


void UCharacterDataComponent::OnRep_CharacterData()
{
	check(CharacterData);

	CheckDefaultInitialization();
}

void UCharacterDataComponent::SetCharacterData(const UCharacterData* NewCharacterData)
{
	auto* Pawn{ GetPawnChecked<APawn>() };

	// Do nothing if CharacterData is already set

	if (CharacterData)
	{
		return;
	}

	// Set CharacterData

	if (Pawn->HasAuthority())
	{
		if (ensure(NewCharacterData))
		{
			CharacterData = NewCharacterData;

			CharacterData->ApplyModifiers(Pawn);

			CheckDefaultInitialization();
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
