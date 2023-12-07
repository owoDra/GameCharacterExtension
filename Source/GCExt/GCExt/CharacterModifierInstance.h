// Copyright (C) 2023 owoDra

#pragma once

#include "CharacterModifierInstance.generated.h"

class APawn;
class UCharacterModifier;
class UCharacterDataComponent;


/**
 * Base class that holds data on changes applied during character setup
 */
UCLASS(Abstract)
class GCEXT_API UCharacterModifierInstance : public UObject
{
	GENERATED_BODY()
public:
	UCharacterModifierInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsSupportedForNetworking() const override { return true; }

public:
	/**
	 * Executed when CharacterData is applied
	 */
	virtual void OnApply(const UCharacterModifier* Data, APawn* Pawn) PURE_VIRTUAL(, );

	/**
	 * Executed when the CharacterData is changed and the character is rebuilt
	 */
	virtual void OnRemoval(APawn* Pawn) PURE_VIRTUAL(, );

};
