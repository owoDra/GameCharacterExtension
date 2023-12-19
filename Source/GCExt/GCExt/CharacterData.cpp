// Copyright (C) 2023 owoDra

#include "CharacterData.h"

#include "CharacterModifier.h"
#include "CharacterModifierSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterData)


UCharacterData::UCharacterData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UCharacterData::ApplyModifiers(APawn* Pawn) const
{
	for (const auto& ModifierSet : ModifierSets)
	{
		if (ensure(ModifierSet))
		{
			ModifierSet->ApplyModifiers(Pawn);
		}
	}

	for (const auto& Modifier : Modifiers)
	{
		if (ensure(Modifier))
		{
			Modifier->OnApply(Pawn);
		}
	}
}
