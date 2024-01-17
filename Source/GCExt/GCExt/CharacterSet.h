// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "Recipe/PendingCharacterRecipeHandle.h"

#include "CharacterSet.generated.h"

class UCharacterInitStateComponent;
class UCharacterRecipe;


/**
 * Bundle data of CharacterRecipe to be added for the character
 */
UCLASS(BlueprintType, Const)
class UCharacterSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCharacterSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// List of CharacterRecipe classes to be added by the character
	// 
	// Tips:
	//	Executed according to the order of the array.
	//
	UPROPERTY(EditDefaultsOnly, Category = "CharacterSet")
	TArray<TSubclassOf<UCharacterRecipe>> CharacterRecipes;

public:
	/**
	 * Add a CharacterRecipe to Character
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Recipes")
	void AddCharacterRecipes(UCharacterInitStateComponent* InitStateComponent, TArray<FPendingCharacterRecipeHandle>& OutHandles) const;

};
