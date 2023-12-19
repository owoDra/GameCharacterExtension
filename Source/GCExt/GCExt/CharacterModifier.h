// Copyright (C) 2023 owoDra

#pragma once

#include "CharacterModifier.generated.h"

class APawn;

/**
 * Base class for setting changes to the character to be applied during character setup
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GCEXT_API UCharacterModifier : public UObject
{
	GENERATED_BODY()
public:
	UCharacterModifier() {}

public:
	/**
	 * Executed when CharacterData is applied
	 */
	virtual void OnApply(APawn* Pawn) const PURE_VIRTUAL(, );

};
