// Copyright (C) 2024 owoDra

#include "CharacterModifierSet.h"

#include "CharacterModifier.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifierSet)


UCharacterModifierSet::UCharacterModifierSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UCharacterModifierSet::ApplyModifiers(APawn* Pawn) const
{
	for (const auto& Modifier : Modifiers)
	{
		if (ensure(Modifier))
		{
			Modifier->OnApply(Pawn);
		}
	}
}
