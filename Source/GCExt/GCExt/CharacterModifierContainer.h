// Copyright (C) 2023 owoDra

#pragma once

#include "Net/Serialization/FastArraySerializer.h"

#include "CharacterModifierContainer.generated.h"

class UCharacterModifier;
class UCharacterModifierInstance;
class UCharacterDataComponent;
class APawn;


/**
 * A single entry in an CharacterModifierContainer
 */
USTRUCT(BlueprintType)
struct FActiveCharacterModifier : public FFastArraySerializerItem
{
	GENERATED_BODY()
private:
	friend struct FCharacterModifierContainer;
	friend class UCharacterDataComponent;

public:
	FActiveCharacterModifier() {}

private:
	//
	// CharacterActionInstance currently applied to Pawn or Character
	//
	UPROPERTY()
	TObjectPtr<const UCharacterModifier> Data{ nullptr };

	//
	// CharacterActionInstance currently applied to Pawn or Character
	//
	UPROPERTY()
	TObjectPtr<UCharacterModifierInstance> Instance{ nullptr };

};


/**
 * List of CharacterModifierInstance currently applied to Pawn or Character
 */
USTRUCT(BlueprintType)
struct FCharacterModifierContainer : public FFastArraySerializer
{
	GENERATED_BODY()
private:
	friend class UCharacterDataComponent;

public:
	FCharacterModifierContainer()
		: OwnerComponent(nullptr)
	{
	}

	FCharacterModifierContainer(UCharacterDataComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

private:
	//
	// List of CharacterModifierInstances currently applied to Pawn or Character
	//
	UPROPERTY()
	TArray<FActiveCharacterModifier> Entries;

	//
	// Components owning this list
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UCharacterDataComponent> OwnerComponent;

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FActiveCharacterModifier, FCharacterModifierContainer>(Entries, DeltaParms, *this);
	}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

public:
	void AddEntry(UCharacterModifier* Modifier, APawn* Pawn);
	void RemoveAllEntries(APawn* Pawn);
	
};

template<>
struct TStructOpsTypeTraits<FCharacterModifierContainer> : public TStructOpsTypeTraitsBase2<FCharacterModifierContainer>
{
	enum { WithNetDeltaSerializer = true };
};
