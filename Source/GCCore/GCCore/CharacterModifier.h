// Copyright (C) 2023 owoDra

#pragma once

#include "CharacterModifier.generated.h"

class APawn;
class UCharacterModifierInstance;

/**
 * Base class for setting changes to the character to be applied during character setup
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GCCORE_API UCharacterModifier : public UObject
{
	GENERATED_BODY()
public:
	UCharacterModifier() {}

public:
	/**
	 * Executed when CharacterData is applied
	 */
	 UCharacterModifierInstance* CreateModifier(APawn* Pawn);

protected:
	/**
	 * return modifier class
	 */
	virtual UClass* GetInstanceClass() const PURE_VIRTUAL(, return nullptr;);

};
