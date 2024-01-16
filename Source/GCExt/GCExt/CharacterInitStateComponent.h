// Copyright (C) 2024 owoDra

#pragma once

#include "InitState/InitStateComponent.h"

#include "Recipe/ActiveCharacterRecipe.h"

#include "CharacterInitStateComponent.generated.h"

class UCharacterRecipe;


/**
 * Component class that manages character initialization
 * 
 * Tips:
 *	CharacterRecipe can be added to operate the character building during initialization
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GCEXT_API UCharacterInitStateComponent : public UInitStateComponent
{
	GENERATED_BODY()
public:
	UCharacterInitStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/////////////////////////////////////////////////////////////////
	// Init State Flows
#pragma region Init State Flows
protected:
	virtual void OnRegister() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const override;
	virtual void HandleChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) override;

	/**
	 * Notify that the Controller has been changed.
	 */
	UFUNCTION()
	virtual void HandleControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

#pragma endregion


	/////////////////////////////////////////////////////////////////
	// Character Recipes
#pragma region Character Recipes
protected:
	//
	// List of CharacterRecipe classes to be added by default
	//
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Recipes")
	TArray<TSubclassOf<UCharacterRecipe>> DefaultCharacterRecipes;

	//
	// Whether to automatically commit CharacterRecipe at spawn
	//
	UPROPERTY(EditAnywhere, Category = "Recipes")
	bool bAutoCommitCharacterRecipes{ false };

	//
	// List of added CharacterRecipes
	//
	UPROPERTY(Transient, ReplicatedUsing = "OnRep_CommitRecipes")
	FActiveCharacterRecipeContainer ActiveCharacterRecipes;

public:
	/**
	 * Add CharacterRecipe class to pending list
	 *
	 * Note:
	 *	Must have authority
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	FPendingCharacterRecipeHandle AddPendingCharacterRecipe(const TSubclassOf<UCharacterRecipe>& InClass);

	/**
	 * Add multiple CharacterRecipe classes to pending list
	 *
	 * Note:
	 *	Must have authority
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	TArray<FPendingCharacterRecipeHandle> AddMultipePendingCharacterRecipes(const TArray<TSubclassOf<UCharacterRecipe>>& InClasses);

	/**
	 * Remove CharacterRecipe class from pending list
	 *
	 * Note:
	 *	Must have authority
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	void RemovePendingCharacterRecipe(const FPendingCharacterRecipeHandle& InHandle);

	/**
	 * Remove multiple CharacterRecipe classes from pending list
	 *
	 * Note:
	 *	Must have authority
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	void RemoveMultipePendingCharacterRecipes(const TArray<FPendingCharacterRecipeHandle>& InHandles);

	/**
	 * Clear all CharacterRecipe classes from pending list
	 * 
	 * Note:
	 *	Must have authority
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	void ClearPendingCharacterRecipes();

	/**
	 * Commit the CharacterRecipe class in the current WaitingList and apply it to the character
	 *
	 * Tips:
	 *	Create an ActiveCharacterRecipe and add it to the container according to the CharacterRecipe Policy
	 *
	 * Note:
	 *	Must have authority
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	void CommitPendingCharacterRecipes();

private:
	/**
	 * Notify that all CharacterRecipe classes have been committed
	 *
	 * Tips:
	 *	Execute the start of the CharacterRecipe process.
	 */
	void HandleAllRecipesCommitted();

	/**
	 * Will be called from CommitRecipes or from OnRep.
	 */
	UFUNCTION()
	void OnRep_CommitRecipes();

	/**
	 * Add default CharacterRecipe to Pending list
	 */
	void AddDefaultCharacterRecipeToPendingList();

	/**
	 * Executed when character will destory, releasing CharacterRecipes.
	 */
	void ReleaseCharacterRecipes();


protected:
	//
	// Timer handle for the delay between the completion of CharacterRecipe processing and init state transition check.
	// 
	// Tips:
	//	This is used to avoid mix-ups when multiple Recipes are completed in the same frame.
	//
	UPROPERTY(Transient)
	FTimerHandle DelayedCheckRecipeSetupFinishedTimerHandle;

public:
	/**
	 * Notify that the processing of CharacterRecipe is complete.
	 */
	void HandleRecipeSetupFinished(const FActiveCharacterRecipeHandle& Handle);

protected:
	/**
	 * Update the end flag of the CharacterRecipe that has been processed in the previous frame and send a signal for synchronization
	 */
	void HandleDelayedCheckRecipeSetupFinished();

#pragma endregion


	/////////////////////////////////////////////////////////////////
	// Utilities
public:
	template <class T>
	T* GetPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template parameter to GetPawn must be derived from APawn");
		return Cast<T>(GetOwner());
	}

	template <class T>
	T* GetPawnChecked() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template parameter to GetPawnChecked must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

};
