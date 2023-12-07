// Copyright (C) 2023 owoDra

#include "CharacterModifierContainer.h"

#include "CharacterDataComponent.h"
#include "CharacterModifierInstance.h"
#include "CharacterModifier.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifierContainer)


void FCharacterModifierContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	auto* Pawn{ OwnerComponent->GetPawnChecked<APawn>() };

	for (const auto& Index : RemovedIndices)
	{
		const auto& Instance{ Entries[Index].Instance };
		if (ensure(Instance))
		{
			Instance->OnRemoval(Pawn);
		}
	}
}

void FCharacterModifierContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	auto* Pawn{ OwnerComponent->GetPawnChecked<APawn>() };

	for (const auto& Index : AddedIndices)
	{
		const auto& Instance{ Entries[Index].Instance };
		const auto& Data{ Entries[Index].Data };

		if (ensure(Instance) && ensure(Data))
		{
			Instance->OnApply(Data, Pawn);
		}
	}
}

void FCharacterModifierContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}


void FCharacterModifierContainer::AddEntry(UCharacterModifier* Modifier, APawn* Pawn)
{
	check(Modifier);
	check(Pawn);
	check(Pawn->HasAuthority());

	auto& NewEntry{ Entries.AddDefaulted_GetRef() };
	NewEntry.Data = Modifier;
	NewEntry.Instance = Modifier->CreateModifier(Pawn);
	NewEntry.Instance->OnApply(Modifier, Pawn);

	MarkItemDirty(NewEntry);
}

void FCharacterModifierContainer::RemoveAllEntries(APawn* Pawn)
{
	check(Pawn);
	check(Pawn->HasAuthority());

	for (auto EntryIt{ Entries.CreateIterator() }; EntryIt; ++EntryIt)
	{
		const auto& Instance{ EntryIt->Instance };
		if (Instance)
		{
			Instance->OnRemoval(Pawn);
		}

		EntryIt.RemoveCurrent();
		MarkArrayDirty();
	}
}
