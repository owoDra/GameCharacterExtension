// Copyright (C) 2024 owoDra

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

protected:
	bool bApplyOnServer{ true };
	bool bApplyOnClient{ true };

	bool bOnlyApplyOnLocal{ false };

public:
	/**
	 * Executed when CharacterData is applied
	 */
	virtual bool OnApply(APawn* Pawn) const;

};
