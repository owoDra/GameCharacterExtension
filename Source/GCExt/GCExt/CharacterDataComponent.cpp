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
	// This component can only be added to classes derived from APawn

	const auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("CharacterDataComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	Super::OnRegister();
}

bool UCharacterDataComponent::CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	return (CharacterData != nullptr);
}


void UCharacterDataComponent::OnRep_CharacterData()
{
	check(CharacterData);

	auto* Pawn{ GetPawnChecked<APawn>() };

	CharacterData->ApplyModifiers(Pawn);

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
