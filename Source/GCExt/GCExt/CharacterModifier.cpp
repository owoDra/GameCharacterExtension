// Copyright (C) 2023 owoDra

#include "CharacterModifier.h"

#include "CharacterModifierInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier)


UCharacterModifierInstance* UCharacterModifier::CreateModifier(APawn* Pawn)
{
	if (auto* Class{ GetInstanceClass() })
	{
		return NewObject<UCharacterModifierInstance>(Pawn, Class);
	}

	return nullptr;
}
