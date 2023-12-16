// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "CharacterModifierSet.generated.h"

class UCharacterModifier;
class APawn;


/**
 * Data collected on CharacterModifier
 * 
 * Tips:
 *	Use when you want to execute a CharacterModifier that is common among multiple CharacterData.
 */
UCLASS(BlueprintType, Const)
class UCharacterModifierSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCharacterModifierSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
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
