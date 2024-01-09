// Copyright (C) 2024 owoDra

#include "CharacterDataComponent.h"

#include "CharacterData.h"
#include "CharacterModifier.h"
#include "GCExtLogs.h"

#include "InitState/InitStateTags.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Pawn.h"
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

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(UCharacterDataComponent, CharacterData, Params);
}


void UCharacterDataComponent::OnRegister()
{
	// This component can only be added to classes derived from APawn

	auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("CharacterDataComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	if (Pawn)
	{
		FScriptDelegate NewDelegate;
		NewDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UCharacterDataComponent, OnControllerChanged));
		Pawn->ReceiveControllerChangedDelegate.Add(NewDelegate);
	}

	Super::OnRegister();
}

bool UCharacterDataComponent::CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	if (!CharacterData)
	{
		return false;
	}

	auto* Pawn{ GetPawnChecked<APawn>() };
	const auto bHasAuthority{ Pawn->HasAuthority() };
	const auto bIsLocallyControlled{ Pawn->IsLocallyControlled() };

	if (bHasAuthority || bIsLocallyControlled)
	{
		// Check for being possessed by a controller.

		if (!Pawn->GetController())
		{
			return false;
		}
	}

	return true;
}

void UCharacterDataComponent::HandleChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager)
{
	auto* Pawn{ GetPawnChecked<APawn>() };

	CharacterData->ApplyModifiers(Pawn);
}


void UCharacterDataComponent::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	if (NewController)
	{
		CheckDefaultInitialization();

		GetPawnChecked<APawn>()->ReceiveControllerChangedDelegate.RemoveAll(this);
	}
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

			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CharacterData, this);

			CheckDefaultInitialization();
		}
	}
}
