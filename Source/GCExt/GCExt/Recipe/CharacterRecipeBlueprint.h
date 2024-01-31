// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/Blueprint.h"

#include "CharacterRecipeBlueprint.generated.h"

/**
 * Blueprint class used to setup character attribute on the editor
 * 
 * Tips:
 *	The character recipe factory should pick this for you automatically
 */
UCLASS(BlueprintType)
class GCEXT_API UCharacterRecipeBlueprint : public UBlueprint
{
	GENERATED_BODY()
public:
	UCharacterRecipeBlueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////
	// Editor Only
public:
#if WITH_EDITOR
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
#endif

};
