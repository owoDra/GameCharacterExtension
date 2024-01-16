// Copyright (C) 2024 owoDra

#pragma once

#include "Net/Serialization/FastArraySerializer.h"

#include "Recipe/ActiveCharacterRecipeHandle.h"
#include "Recipe/PendingCharacterRecipeHandle.h"

#include "ActiveCharacterRecipe.generated.h"

class APawn;
class UCharacterRecipe;
class UCharacterInitStateComponent;


/**
 * Current CharacterRecipes application state
 */
UENUM(BlueprintType)
enum class ECharacterRecipesApplicationState : uint8
{
	// CharacterRecipe classes not yet committed
	PreCommit,

	// CharacterRecipe classes have been committed and during the ActiveCharacterRecipe setup process.
	Commited,

	// All ActiveCharacterRecipe setup processes are complete.
	Complete
};


/**
 * Data of the CharacterRecipe currently applied to the character
 */
USTRUCT(BlueprintType)
struct GCEXT_API FActiveCharacterRecipe : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend struct FActiveCharacterRecipeContainer;

public:
	FActiveCharacterRecipe() {}

	/** 
	 * Version that takes an CharacterRecipe class 
	 */
	FActiveCharacterRecipe(TSubclassOf<UCharacterRecipe> InClass);

	/** 
	 * Version that takes an CharacterRecipe CDO
	 */
	FActiveCharacterRecipe(const UCharacterRecipe* InCDO);


protected:
	//
	// Unique handle to identify this ActiveCharacterRecipe
	//
	UPROPERTY()
	FActiveCharacterRecipeHandle Handle;

	//
	// CDO of the CharacterRecipe
	// 
	// Tips:
	//	If InstancingPolicy is "NonInstanced", this CDO is used for processing.
	//
	UPROPERTY()
	TObjectPtr<const UCharacterRecipe> RecipeCDO{ nullptr };

	//
	// Instanced of the CharacterRecipe
	// 
	// Tips:
	//	If InstancingPolicy is "Instanced", this Instance is used for processing.
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UCharacterRecipe> RecipeInstance{ nullptr };

	//
	// Whether the process is complete or not
	//
	UPROPERTY(NotReplicated)
	bool bFinished{ false };

protected:
	/**
	 * Notify that a CharacterRecipe has been committed and an ActiveCharacterRecipe has been created.
	 */
	void HandleCharacterRecipeComitted(APawn* Owner, bool bHasAuthority, bool bLocallyControlled, bool bIsDedicatedServer);

	/**
	 * Create instances as needed
	 */
	void TryCreateInstance(APawn* Owner, bool bHasAuthority, bool bLocallyControlled, bool bIsDedicatedServer);

	/**
	 * Perform setup process with CharacterRecipe if possible
	 */
	void TryExecuteSetup(APawn* Owner, UCharacterInitStateComponent* OwnerComponent, bool bHasAuthority, bool bLocallyControlled, bool bIsDedicatedServer);

	/**
	 * Mark as finished
	 */
	void MarkFinished();

	/**
	 * Notify the character to be destroyed.
	 */
	void NotifyDestroy();

public:
	/**
	 * Returns debug string of this
	 */
	FString GetDebugString();

};


/**
 * List of ActiveCharacterRecipe
 */
USTRUCT(BlueprintType)
struct GCEXT_API FActiveCharacterRecipeContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FActiveCharacterRecipeContainer() {}

	void RegisterOwner(APawn* InOwner, UCharacterInitStateComponent* InOwnerComponent);

public:
	//
	// List of currently applied ActiveCharacterRecipes
	//
	UPROPERTY()
	TArray<FActiveCharacterRecipe> Entries;

	//
	// Mapping List of currently pending CharacterRecipe classes
	// 
	// Tips:
	//	Basically only referenced in environments with Authority
	//
	UPROPERTY(NotReplicated)
	TMap<FPendingCharacterRecipeHandle, TSubclassOf<UCharacterRecipe>> PendingRecipeMap;

	//
	// List of ActiveCharacterRecipeHandle pending finish
	//
	UPROPERTY(NotReplicated)
	TSet<FActiveCharacterRecipeHandle> RecipesPendingFinish;

	//
	// The owner of this container
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<APawn> Owner{ nullptr };

	//
	// The owner of this container
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UCharacterInitStateComponent> OwnerComponent{ nullptr };

	//
	// Current CharacterRecipes application state
	// 
	// Tips:
	//	This value is not replicated, but is updated when the container is pushed and replicated to the client
	//
	UPROPERTY(NotReplicated)
	mutable ECharacterRecipesApplicationState ApplicationState{ ECharacterRecipesApplicationState::PreCommit };

public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FActiveCharacterRecipe, FActiveCharacterRecipeContainer>(Entries, DeltaParms, *this);
	}


public:
	/**
	 * Add a new CharacterRecipe class to the Pending list
	 */
	FPendingCharacterRecipeHandle AddPendingCharacterRecipe(TSubclassOf<UCharacterRecipe> CharacterRecipe);

	/**
	 * Delete the CharacterRecipe class of the specified handle from the Pending list
	 */
	void RemovePendingCharacterRecipe(const FPendingCharacterRecipeHandle& Handle);

	/**
	 * Clear CharacterRecipe classes from the Pending list
	 */
	void ClearPendingCharacterRecipes();

	/**
	 * Commit CharacterRecipes from the Pending list
	 */
	void CommitPendingCharacterRecipes();

	/**
	 * Start the setup process for CharacterRecipe
	 */
	void ExecuteCharacterRecipeSetup();

	/**
	 * Add a new ActiveCharacterRecipeHandle to the Pending list
	 */
	void AddActiveRecipeHandlePendingFinish(const FActiveCharacterRecipeHandle& InHandle);

	/**
	 * Commit ActiveCharacterRecipe finish from the Pending list
	 */
	void MarkActiveRecipeHandlePendingFinish();

	/**
	 * Executed when character will destory, releasing CharacterRecipes.
	 */
	void ReleaseCharacterRecipes();

public:
	/**
	 * Returns current CharacterRecipes application state
	 */
	ECharacterRecipesApplicationState GetCurrentApplicationState() const;

};

template<>
struct TStructOpsTypeTraits<FActiveCharacterRecipeContainer> : public TStructOpsTypeTraitsBase2<FActiveCharacterRecipeContainer>
{
	enum { WithNetDeltaSerializer = true };
};
