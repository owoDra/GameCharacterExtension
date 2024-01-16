// Copyright (C) 2024 owoDra

#include "ActiveCharacterRecipe.h"

#include "Recipe/CharacterRecipe.h"
#include "GCExtLogs.h"

#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActiveCharacterRecipe)


//////////////////////////////////////////////////////
// FActiveCharacterRecipe

#pragma region FActiveCharacterRecipe

FActiveCharacterRecipe::FActiveCharacterRecipe(TSubclassOf<UCharacterRecipe> InClass)
	: RecipeCDO(InClass ? InClass.GetDefaultObject() : nullptr)
{
	Handle.GenerateNewHandle();
}

FActiveCharacterRecipe::FActiveCharacterRecipe(const UCharacterRecipe* InCDO)
	: RecipeCDO(InCDO)
{
	Handle.GenerateNewHandle();
}


void FActiveCharacterRecipe::HandleCharacterRecipeComitted(APawn* Owner, bool bHasAuthority, bool bLocallyControlled)
{
	TryCreateInstance(Owner, bHasAuthority, bLocallyControlled);

	UE_LOG(LogGCE_Recipes, Log, TEXT("\n[%s|%s] Committed: %s")
		, bHasAuthority ? TEXT("SERVER") : TEXT("CLIENT")
		, bLocallyControlled ? TEXT("Local") : TEXT("NotLocal")
		, *GetDebugString());
}

void FActiveCharacterRecipe::TryCreateInstance(APawn* Owner, bool bHasAuthority, bool bLocallyControlled)
{
	if (RecipeCDO && RecipeCDO->GetInstancingPolicy() == ECharacterRecipeInstancingPolicy::Instanced)
	{
		const auto ExecutionPolicy{ RecipeCDO->GetNetExecutionPolicy() };

		if ((ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::Both)
			|| (bHasAuthority && ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::ServerOnly)
			|| (bLocallyControlled && ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::LocalOnly)
			|| ((bLocallyControlled || !bHasAuthority) && ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::ClientOnly))
		{
			RecipeInstance = NewObject<UCharacterRecipe>(Owner, RecipeCDO->GetClass());
		}
	}
}

void FActiveCharacterRecipe::TryExecuteSetup(APawn* Owner, UCharacterInitStateComponent* OwnerComponent, bool bHasAuthority, bool bLocallyControlled)
{
	auto PawnInfo{ FCharacterRecipePawnInfo(Handle, Owner, OwnerComponent) };
	const auto ExecutionPolicy{ RecipeCDO->GetNetExecutionPolicy() };

	// Execute if possible.

	if ((ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::Both)
		|| (bHasAuthority && ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::ServerOnly)
		|| (bLocallyControlled && ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::LocalOnly)
		|| ((bLocallyControlled || !bHasAuthority) && ExecutionPolicy == ECharacterRecipeNetExecutionPolicy::ClientOnly))
	{
		if (RecipeCDO->GetInstancingPolicy() == ECharacterRecipeInstancingPolicy::Instanced)
		{
			if (RecipeInstance)
			{
				RecipeInstance->HandleStartSetup(PawnInfo);
			}
			else
			{
				UE_LOG(LogGCE_Recipes, Error, TEXT("%s | Tried to execute, but no instance has been created"), *GetDebugString());
			}
		}
		else
		{
			RecipeCDO->HandleStartSetupNonInstanced(PawnInfo);
		}
	}

	// Mark as finished if not needed to run in the current environment

	else
	{
		MarkFinished();
	}
}

void FActiveCharacterRecipe::MarkFinished()
{
	bFinished = true;
}

void FActiveCharacterRecipe::NotifyDestroy()
{
	if (RecipeCDO && RecipeCDO->GetInstancingPolicy() == ECharacterRecipeInstancingPolicy::Instanced)
	{
		if (RecipeInstance)
		{
			RecipeInstance->HandleDestroy();
		}
	}
}


FString FActiveCharacterRecipe::GetDebugString()
{
	return FString::Printf(TEXT("[%u](CDO:%s, Instance:%s)"),
		*Handle.ToString(), *GetNameSafe(RecipeCDO), *GetNameSafe(RecipeInstance));
}

#pragma endregion


//////////////////////////////////////////////////////
// FActiveCharacterRecipeContainer

#pragma region FActiveCharacterRecipeContainer

void FActiveCharacterRecipeContainer::RegisterOwner(APawn* InOwner, UCharacterInitStateComponent* InOwnerComponent)
{
	Owner = InOwner;
	OwnerComponent = InOwnerComponent;
}


void FActiveCharacterRecipeContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FActiveCharacterRecipeContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	check(Owner);

	const auto bHasAuthority{ Owner->HasAuthority() };
	const auto bLocallyControlled{ Owner->IsLocallyControlled() };

	for (const auto& Index : AddedIndices)
	{
		auto& Entry{ Entries[Index] };

		Entry.HandleCharacterRecipeComitted(Owner, bHasAuthority, bLocallyControlled);
	}
}

void FActiveCharacterRecipeContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}


FPendingCharacterRecipeHandle FActiveCharacterRecipeContainer::AddPendingCharacterRecipe(TSubclassOf<UCharacterRecipe> CharacterRecipe)
{
	if (CharacterRecipe)
	{
		FPendingCharacterRecipeHandle NewHandle;
		NewHandle.GenerateNewHandle();

		PendingRecipeMap.Add(NewHandle, CharacterRecipe);

		return NewHandle;
	}

	return FPendingCharacterRecipeHandle();
}

void FActiveCharacterRecipeContainer::RemovePendingCharacterRecipe(const FPendingCharacterRecipeHandle& Handle)
{
	PendingRecipeMap.Remove(Handle);
}

void FActiveCharacterRecipeContainer::ClearPendingCharacterRecipes()
{
	PendingRecipeMap.Empty();
}

void FActiveCharacterRecipeContainer::CommitPendingCharacterRecipes()
{
	check(Owner);

	const auto bHasAuthority{ Owner->HasAuthority() };
	const auto bLocallyControlled{ Owner->IsLocallyControlled() };

	// Create an ActiveCharacterRecipe based on the CharacterRecipe class registered in PendingRecipeMap

	for (const auto& KVP : PendingRecipeMap)
	{
		const auto& PendingRecipe{ KVP.Value };

		if (PendingRecipe)
		{
			auto& NewActiveRecipe{ Entries.Emplace_GetRef(PendingRecipe) };
			NewActiveRecipe.HandleCharacterRecipeComitted(Owner, bHasAuthority, bLocallyControlled);

			MarkItemDirty(NewActiveRecipe);
		}
		else
		{
			UE_LOG(LogGCE_Recipes, Error, TEXT("Invalid CharacterRecipe class was registered in PendingRecipeMap"));
		}
	}

	PendingRecipeMap.Empty();

	// To cause a replicate event even if there is no CharacterRecipe

	MarkArrayDirty();
}

void FActiveCharacterRecipeContainer::ExecuteCharacterRecipeSetup()
{
	check(Owner);
	check(OwnerComponent);

	const auto bHasAuthority{ Owner->HasAuthority() };
	const auto bLocallyControlled{ Owner->IsLocallyControlled() };

	for (auto& Entry : Entries)
	{
		Entry.TryExecuteSetup(Owner, OwnerComponent, bHasAuthority, bLocallyControlled);
	}
}

void FActiveCharacterRecipeContainer::AddActiveRecipeHandlePendingFinish(const FActiveCharacterRecipeHandle& InHandle)
{
	RecipesPendingFinish.Add(InHandle);
}

void FActiveCharacterRecipeContainer::MarkActiveRecipeHandlePendingFinish()
{
	for (auto& Entry : Entries)
	{
		if (RecipesPendingFinish.Contains(Entry.Handle))
		{
			Entry.MarkFinished();
		}
	}

	RecipesPendingFinish.Empty();
}

void FActiveCharacterRecipeContainer::ReleaseCharacterRecipes()
{
	Entries.Empty();
	PendingRecipeMap.Empty();
	RecipesPendingFinish.Empty();

	for (auto& Entry : Entries)
	{
		Entry.NotifyDestroy();
	}

	UE_LOG(LogGCE_Recipes, Log, TEXT("\n[%s] All CharacterRecipes Released"), Owner->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));

	/**
	 * This process is automatically called by the server and client during the InitState flow, 
	 * so there is no need to execute MarkArrayDirty().
	 */
	// MarkArrayDirty();
}


ECharacterRecipesApplicationState FActiveCharacterRecipeContainer::GetCurrentApplicationState() const
{
	if (ApplicationState == ECharacterRecipesApplicationState::Commited)
	{
		for (const auto& Entry : Entries)
		{
			if (!Entry.bFinished)
			{
				return ApplicationState;
			}
		}

		ApplicationState = ECharacterRecipesApplicationState::Complete;
	}

	return ApplicationState;
}

#pragma endregion
