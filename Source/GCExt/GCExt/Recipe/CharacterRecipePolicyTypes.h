// Copyright (C) 2024 owoDra

#pragma once

#include "CharacterRecipePolicyTypes.generated.h"


/**
 * How the CharacterRecipe is instanced when applied. 
 * 
 * Tips:
 *	This limits what an CharacterRecipe can do in its implementation. 
 *	For example, a NonInstanced CharacterRecipe cannot have state.
 */
UENUM(BlueprintType)
enum class ECharacterRecipeInstancingPolicy : uint8
{
	// This CharacterRecipe is never instanced. 
	// Anything that executes the ability is operating on the CDO.
	NonInstanced,

	// Each apwn gets their own instance of this CharacterRecipe. 
	// State can be saved, replication is possible.
	Instanced,
};


/**
 * Where does an CharacterRecipe execute on the network. 
 */
UENUM(BlueprintType)
enum class ECharacterRecipeNetExecutionPolicy : uint8
{
	// This CharacterRecipe will only both server and client
	Both,

	// This CharacterRecipe will only run on the server
	ServerOnly,

	// This CharacterRecipe will only run on the client
	ClientOnly,

	// This CharacterRecipe will only run on the local client or server that has local control
	LocalOnly
};
