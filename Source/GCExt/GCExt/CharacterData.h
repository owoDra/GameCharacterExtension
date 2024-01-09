// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "CharacterData.generated.h"

class UCharacterModifier;
class UCharacterModifierSet;
class APawn;


/**
 * Data used to spawn Character operated by players or AI
 * 
 * Tips:
 *	CharacterData represents the character itself. 
 *	For example, in a game with multiple characters to choose from,
 *	there should be as much CharacterData as there are characters.
 * 
 * Note:
 *	CharacterModifier is executed first in ModifierSets and then in Modifiers.
 */
UCLASS(BlueprintType, Const)
class UCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCharacterData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// List of additional processes to be performed when creating a Character or Pawn
	// 
	// Tips:
	//	This CharacterModifierSet is executed according to the order of the array.
	//
	UPROPERTY(EditDefaultsOnly, Category = "CharacterData")
	TArray<TObjectPtr<UCharacterModifierSet>> ModifierSets;

	//
	// List of additional processes to be performed when creating a Character or Pawn
	// 
	// Tips:
	//	This CharacterModifier is executed according to the order of the array.
	//
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "CharacterData")
	TArray<TObjectPtr<UCharacterModifier>> Modifiers;

public:
	/**
	 * Apply Modifiers
	 */
	void ApplyModifiers(APawn* Pawn) const;

};
